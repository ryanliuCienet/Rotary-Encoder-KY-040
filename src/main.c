#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

// 定義GPIO配置
#define ROT_A_NODE DT_PATH(rotary_encoder, rotary_a)
#define ROT_B_NODE DT_PATH(rotary_encoder, rotary_b)
#define ROT_BTN_NODE DT_PATH(rotary_encoder, rotary_button)

// 定義GPIO設備
static const struct gpio_dt_spec rot_a = GPIO_DT_SPEC_GET(ROT_A_NODE, gpios);
static const struct gpio_dt_spec rot_b = GPIO_DT_SPEC_GET(ROT_B_NODE, gpios);
static const struct gpio_dt_spec rot_btn = GPIO_DT_SPEC_GET(ROT_BTN_NODE, gpios);

// 計數器變量
static int counter = 0;
static int last_a_state;

// GPIO callback數據結構
static struct gpio_callback rot_a_cb_data;
static struct gpio_callback rot_btn_cb_data;

// 旋轉回調函數
static void rotary_changed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    int a_state = gpio_pin_get_dt(&rot_a);
    int b_state = gpio_pin_get_dt(&rot_b);

    if (a_state != last_a_state) {
        if (b_state != a_state) {
            counter++;
            printk("Clockwise rotation, counter: %d\n", counter);
        } else {
            counter--;
            printk("Counter-clockwise rotation, counter: %d\n", counter);
        }
    }
    last_a_state = a_state;
}

// 按鈕回調函數
static void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    printk("Button pressed! Counter reset to 0\n");
    counter = 0;
}

void main(void)
{
    int ret;

    // 檢查設備是否就緒
    if (!device_is_ready(rot_a.port) ||
        !device_is_ready(rot_b.port) ||
        !device_is_ready(rot_btn.port)) {
        printk("Error: GPIO device not ready\n");
        return;
    }

    // 配置GPIO
    ret = gpio_pin_configure_dt(&rot_a, GPIO_INPUT);
    if (ret < 0) {
        printk("Error: Rotary A pin configuration failed\n");
        return;
    }

    ret = gpio_pin_configure_dt(&rot_b, GPIO_INPUT);
    if (ret < 0) {
        printk("Error: Rotary B pin configuration failed\n");
        return;
    }

    ret = gpio_pin_configure_dt(&rot_btn, GPIO_INPUT);
    if (ret < 0) {
        printk("Error: Button pin configuration failed\n");
        return;
    }

    // 設置中斷
    ret = gpio_pin_interrupt_configure_dt(&rot_a, GPIO_INT_EDGE_BOTH);
    if (ret < 0) {
        printk("Error: Rotary A interrupt configuration failed\n");
        return;
    }

    ret = gpio_pin_interrupt_configure_dt(&rot_btn, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret < 0) {
        printk("Error: Button interrupt configuration failed\n");
        return;
    }

    // 初始化回調
    gpio_init_callback(&rot_a_cb_data, rotary_changed, BIT(rot_a.pin));
    gpio_init_callback(&rot_btn_cb_data, button_pressed, BIT(rot_btn.pin));

    // 添加回調
    gpio_add_callback(rot_a.port, &rot_a_cb_data);
    gpio_add_callback(rot_btn.port, &rot_btn_cb_data);

    // 獲取初始狀態
    last_a_state = gpio_pin_get_dt(&rot_a);

    printk("Rotary encoder initialized\n");

    while (1) {
        k_sleep(K_MSEC(100));
    }
}