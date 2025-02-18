
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define JOYSTICK_X_PIN 26 // GPIO para eixo X
#define JOYSTICK_Y_PIN 27 // GPIO para eixo Y
#define JOYSTICK_PB 22    // GPIO para botão do Joystick
#define Botao_A 5         // GPIO para botão A
#define botaoB 6          // GPIO para botão B

#define LED_R 13 // GPIO do LED vermelho
#define LED_G 11 // GPIO do LED verde
#define LED_B 12 // GPIO do LED azul

volatile bool toggle_green_led = false;
volatile bool toggle_leds = true;
volatile uint8_t border_style = 1;

// Declaração da função pwm_set_duty
void pwm_set_duty(uint gpio, uint16_t value);

// Implementação da função
void pwm_set_duty(uint gpio, uint16_t value)
{
    pwm_set_gpio_level(gpio, value);
}

// Função de interrupção para os botões
void gpio_callback(uint gpio, uint32_t events)
{
    static absolute_time_t last_time = {0};
    absolute_time_t now = get_absolute_time();

    // Debounce de 200ms
    if (absolute_time_diff_us(last_time, now) < 200000)
    {
        return;
    }
    last_time = now;

    if (gpio == JOYSTICK_PB)
    {
        toggle_green_led = !toggle_green_led;
        pwm_set_duty(LED_G, toggle_green_led ? 2000 : 0);
        border_style = (border_style == 1) ? 2 : 1;
    }
    else if (gpio == Botao_A)
    {
        toggle_leds = !toggle_leds;
        if (!toggle_leds)
        {
            pwm_set_duty(LED_R, 0);
            pwm_set_duty(LED_B, 0);
        }
    }
}

int main()
{
    stdio_init_all();

    // Para ser utilizado o modo BOOTSEL com botão B
    gpio_init(botaoB);
    gpio_set_dir(botaoB, GPIO_IN);
    gpio_pull_up(botaoB);
    gpio_set_irq_enabled_with_callback(botaoB, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    gpio_init(JOYSTICK_PB);
    gpio_set_dir(JOYSTICK_PB, GPIO_IN);
    gpio_pull_up(JOYSTICK_PB);

    gpio_init(Botao_A);
    gpio_set_dir(Botao_A, GPIO_IN);
    gpio_pull_up(Botao_A);

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA);                                        // Pull up the data line
    gpio_pull_up(I2C_SCL);                                        // Pull up the clock line
    ssd1306_t ssd;                                                // Inicializa a estrutura do display
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd);                                         // Configura o display
    ssd1306_send_data(&ssd);                                      // Envia os dados para o display

    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);

    int16_t x_pos;
    int16_t y_pos;
    char str_x[5];
    char str_y[5];

    const uint16_t CENTRO_X = 1870;
    const uint16_t CENTRO_Y = 1969;
    const uint16_t ZONA_MORTA = 200;
    const int VELOCIDADE_MAX = 5;

    // Posição central do display
    const int CENTRO_DISPLAY_X = 64;
    const int CENTRO_DISPLAY_Y = 32;

    // Posição inicial da letra A (centro do display)
    int pos_x = CENTRO_DISPLAY_X;
    int pos_y = CENTRO_DISPLAY_Y;
    const uint8_t QUADRADO_SIZE = 8; // Você pode alterar este valor para mudar o tamanho
                                     // Cálculo das margens baseadas no tamanho do quadrado
    const uint8_t MARGEM = 2;        // Espaço mínimo entre o quadrado e a borda
    const uint8_t LIMITE_X_MIN = MARGEM;
    const uint8_t LIMITE_X_MAX = WIDTH - QUADRADO_SIZE - MARGEM;
    const uint8_t LIMITE_Y_MIN = MARGEM;
    const uint8_t LIMITE_Y_MAX = HEIGHT - QUADRADO_SIZE - MARGEM;
    bool cor = true;

    // Configura os LEDs para modo PWM
    gpio_set_function(LED_R, GPIO_FUNC_PWM);
    gpio_set_function(LED_G, GPIO_FUNC_PWM);
    gpio_set_function(LED_B, GPIO_FUNC_PWM);

    // Configura os botões
    gpio_set_irq_enabled_with_callback(JOYSTICK_PB, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled(Botao_A, GPIO_IRQ_EDGE_FALL, true);

    // Configuração do PWM para os LEDs
    uint slice_num_r = pwm_gpio_to_slice_num(LED_R);
    uint slice_num_g = pwm_gpio_to_slice_num(LED_G);
    uint slice_num_b = pwm_gpio_to_slice_num(LED_B);

    // Configura PWM 50Hz para todos os LEDs
    pwm_set_clkdiv(slice_num_r, 1250);
    pwm_set_clkdiv(slice_num_g, 1250);
    pwm_set_clkdiv(slice_num_b, 1250);

    pwm_set_wrap(slice_num_r, 2000);
    pwm_set_wrap(slice_num_g, 2000);
    pwm_set_wrap(slice_num_b, 2000);

    // Habilita o PWM para todos os LEDs
    pwm_set_enabled(slice_num_r, true);
    pwm_set_enabled(slice_num_g, true);
    pwm_set_enabled(slice_num_b, true);

    while (true)
    {
        // Lê valores do joystick
        adc_select_input(0); // Eixo X
        uint16_t adc_x = adc_read();
        adc_select_input(1); // Eixo Y
        uint16_t adc_y = adc_read();

        // Atualiza posição do quadrado com limites
        x_pos = ((adc_y * (WIDTH - 24)) / 4095) + 8;            // Adiciona margem horizontal
        y_pos = HEIGHT - 16 - ((adc_x * (HEIGHT - 24)) / 4095); // Adiciona margem vertical

        // Garante que o quadrado fique dentro dos limites
        if (x_pos < 8)
            x_pos = 8; // Limite esquerdo
        if (x_pos > WIDTH - 16)
            x_pos = WIDTH - 16; // Limite direito
        if (y_pos < 8)
            y_pos = 8; // Limite superior
        if (y_pos > HEIGHT - 16)
            y_pos = HEIGHT - 16; // Limite inferior

        // Atualiza display
        ssd1306_fill(&ssd, false);
        ssd1306_rect(&ssd, y_pos, x_pos, QUADRADO_SIZE, QUADRADO_SIZE, true, true);

        // Desenha borda com espessura variável
        if (border_style == 1)
        {
            // Borda fina (1 pixel)
            ssd1306_rect(&ssd, 0, 0, WIDTH, HEIGHT, true, false);
        }
        else
        {
            // Borda grossa (2 pixels)
            ssd1306_rect(&ssd, 0, 0, WIDTH, HEIGHT, true, false);
            ssd1306_rect(&ssd, 1, 1, WIDTH - 2, HEIGHT - 2, true, false);
        }

        // Converte valores ADC para string e exibe
        sprintf(str_x, "%d", adc_x);
        sprintf(str_y, "%d", adc_y);
        //ssd1306_draw_string(&ssd, str_x, 8, 52);
       // ssd1306_draw_string(&ssd, str_y, 49, 52);

        ssd1306_send_data(&ssd);

        // Controle dos LEDs PWM (corrigido vermelho para X e azul para Y)
        if (toggle_leds)
        {
            int32_t dist_x = abs(2048 - adc_x); // Distância do centro para eixo X (LED vermelho)
            int32_t dist_y = abs(2048 - adc_y); // Distância do centro para eixo Y (LED azul)
            const int32_t deadzone = 300;

            // LED Vermelho controlado pelo eixo X
            uint16_t duty_r = 0;
            if (dist_y > deadzone)
            {
                duty_r = ((dist_y - deadzone) * 2000) / (2048 - deadzone);
                if (duty_r > 2000)
                    duty_r = 2000;
            }

            // LED Azul controlado pelo eixo Y
            uint16_t duty_b = 0;
            if (dist_x > deadzone)
            {
                duty_b = ((dist_x - deadzone) * 2000) / (2048 - deadzone);
                if (duty_b > 2000)
                    duty_b = 2000;
            }

            pwm_set_duty(LED_R, duty_r); // LED vermelho com eixo X
            pwm_set_duty(LED_B, duty_b); // LED azul com eixo Y
        }

        sleep_ms(20); // 50Hz de atualização
    }
}
