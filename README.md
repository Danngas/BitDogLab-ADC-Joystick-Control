<div align="center">
  <img src="EmbarcaTech_logo_Azul-1030x428.png" alt="EmbarcaTech Logo" width="600"/>
</div>

# 🎮 BitDogLab ADC Joystick Control

Daniel Silva de Souza

### [Video Demonstrativo do Projeto](https://www.youtube.com/watch?v=dQw4w9WgXcQ)

## 📌 Descrição

O **BitDogLab ADC Joystick Control** é um projeto que explora o uso de **conversores analógico-digitais (ADC)** no RP2040, utilizando um joystick para controlar LEDs RGB e um display OLED. O projeto demonstra a integração de múltiplos periféricos e protocolos de comunicação.

### 🛠 Construído com

- **Microcontrolador:** RP2040 (Placa BitDogLab)
- **Linguagem:** C/C++
- **Frameworks:** Pico SDK
- **Componentes:** LED RGB, Joystick, Display SSD1306, Botões

### 🎯 Objetivos

- Compreender o funcionamento do ADC no RP2040
- Implementar controle PWM para LEDs RGB
- Utilizar comunicação I2C com display OLED
- Aplicar conceitos de interrupções e debouncing

## 📌 Funcionalidades

✔️ **Controle de LEDs RGB via Joystick:**

- LED Azul controlado pelo eixo Y
- LED Vermelho controlado pelo eixo X
- Intensidade variável baseada na posição do joystick

✔️ **Display OLED:**

- Exibição de quadrado 8x8 pixels móvel
- Posição controlada pelo joystick
- Diferentes estilos de borda

✔️ **Controles:**

- Botão do Joystick: Alterna LED Verde e estilo da borda
- Botão A: Ativa/desativa LEDs PWM

## 🔧 Requisitos de Hardware

- Placa BitDogLab
- LED RGB (GPIOs 11, 12, 13)
- Joystick (ADC GPIOs 26, 27)
- Botão do Joystick (GPIO 22)
- Botão A (GPIO 5)
- Display SSD1306 (I2C GPIOs 14, 15)

## 📁 Estrutura do Projeto

### 🔹 Arquivos Principais

- `main.c` - Programa principal
- `display.c` - Controle do display SSD1306
- `led_control.c` - Funções de controle dos LEDs
- `adc.c` - Configuração e leitura do ADC

## ⚙️ Como Usar

1. **Clone o repositório**

```bash
git clone git@github.com:Danngas/BitDogLab-ADC-Joystick-Control.git
cd bitdoglab_control
```

2. **Compile o projeto**

```sh
mkdir build && cd build
cmake ..
make
```

3. **Carregue o firmware**

- Conecte a placa BitDogLab
- Copie o arquivo .uf2 gerado para a placa

## 🎥 Demonstração

O vídeo demonstrativo mostra:

- Funcionamento do controle via joystick
- Resposta dos LEDs RGB
- Movimentação do quadrado no display
- Diferentes estilos de borda

## 📜 Licença

Este projeto está sob a licença MIT.

## 📞 Contato

Para mais informações, entre em contato através de:

- [Seu Email]
- [Seu LinkedIn]
- [Outros contatos relevantes]


## 🔍 Detalhamento do Código

### 💡 Estratégias Principais

#### Controle do Display OLED
- **Inicialização I2C:**
  - Configuração do barramento I2C a 400KHz
  - Utilização dos pinos GPIO 14 (SDA) e 15 (SCL)
  - Pull-up nas linhas de dados e clock

#### Gerenciamento do Joystick
- **Leitura ADC:**
  - Amostragem dos eixos X e Y (GPIOs 26 e 27)
  - Conversão de valores (0-4095) para coordenadas do display
  - Implementação de zona morta (deadzone) de 300 unidades

#### Sistema de LEDs RGB
- **Controle PWM:**
  - Frequência base de 50Hz (clkdiv: 1250)
  - Range de intensidade: 0-2000
  - Mapeamento não-linear para melhor resposta visual

### 🛠 Funções Principais

#### `gpio_callback()`
- **Interrupção GPIO:**
  - Captura de eventos do joystick
  - Debouncing com delay de 20ms
  - Atualização dos LEDs RGB com base na posição do joystick

#### `oled_init()`
- **Inicialização do display:**
  - Configuração do barramento I2C
  - Inicialização do display SSD1306
  - Limpa o display

#### `oled_write_string()`  
- **Escrita de strings no display:**
  - Escrita de strings no display OLED
  - Utilização da biblioteca SSD1306

#### `pwm_set_duty()`
- **Controle PWM:** 
  - Configuração do PWM com frequência base de 50Hz
  - Mapeamento de valores (0-2000) para a faixa de PWM
  - Aplicação de ganho para melhor resposta visual

#### `adc_read()`
- **Leitura ADC:**  
  - Amostragem do eixo X (GPIO 26)
  - Amostragem do eixo Y (GPIO 27)
  - Conversão de valores (0-4095) para coordenadas do display
  - Aplicação de zona morta (deadzone) de 300 unidades

#### `main()`       
- **Função principal:**
  - Inicialização de periféricos
  - Configuração de interrupções
  - Loop principal de controle

- Taxa de atualização: 50Hz (20ms)
- Sequência de operações:
  1. Leitura dos valores ADC
  2. Cálculo das posições do quadrado
  3. Atualização do display
  4. Controle de intensidade dos LEDs

### 🔒 Tratamento de Segurança

- **Limites de Tela:**
  - Implementação de margens seguras
  - Prevenção de overflow nas coordenadas
  - Validação de valores ADC

- **Debounce:**
  - Filtro temporal para botões
  - Prevenção de acionamentos falsos
  - Tempo mínimo entre eventos: 200ms

### 📊 Cálculos e Conversões

#### Mapeamento do Joystick
- Conversão de valores ADC (0-4095) para coordenadas do display
- Fórmulas de mapeamento:
  ```c
  x_pos = ((adc_y * (WIDTH - 24)) / 4095) + 8
  y_pos = HEIGHT - 16 - ((adc_x * (HEIGHT - 24)) / 4095)
  ```

#### Intensidade dos LEDs
- Cálculo baseado na distância do centro:
  ```c
  duty = ((distância - deadzone) * 2000) / (2048 - deadzone)
  ```

## 🎯 Conclusão e Resultados

### 📊 Objetivos Alcançados
- ✅ Implementação bem-sucedida do controle ADC
- ✅ Interface gráfica responsiva no display OLED
- ✅ Sistema de controle PWM para LEDs RGB
- ✅ Tratamento eficiente de interrupções e debounce

### 💡 Aprendizados
Este projeto proporcionou experiência prática com:
- Programação de microcontroladores RP2040
- Protocolos de comunicação (I2C)
- Técnicas de controle PWM
- Gerenciamento de interrupções
- Desenvolvimento de interfaces gráficas em displays OLED

### 🚀 Possíveis Melhorias Futuras
1. **Otimizações:**
   - Implementação de DMA para leituras ADC
   - Melhorias na resposta do joystick
   - Adição de novos efeitos visuais

2. **Novas Funcionalidades:**
   - Modos de operação adicionais
   - Animações no display
   - Integração com outros sensores

3. **Interface:**
   - Menu de configurações
   - Calibração do joystick
   - Perfis de usuário

### 📝 Considerações Finais
O projeto demonstrou ser uma excelente plataforma para aprendizado de sistemas embarcados, combinando aspectos de hardware e software. A experiência adquirida serve como base sólida para projetos mais complexos envolvendo microcontroladores e interfaces homem-máquina.

---

## 🤝 Contribuições

Contribuições são sempre bem-vindas! Sinta-se à vontade para:
- Reportar bugs
- Sugerir novas funcionalidades
- Melhorar a documentação
- Enviar pull requests

---

<div align="center">
  <p>Desenvolvido com 💙 como parte do programa EmbarcaTech</p>
</div>