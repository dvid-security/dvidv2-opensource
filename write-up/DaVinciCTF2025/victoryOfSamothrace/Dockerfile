FROM python:3.9-slim

RUN apt-get update && apt-get install -y \
    git \
    curl \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

RUN pip install platformio

WORKDIR /app

RUN git clone https://github.com/stm32duino/Arduino_Core_STM32.git /tmp/Arduino_Core_STM32

RUN platformio platform install espressif32

COPY . .

RUN mkdir -p ~/.platformio/platforms/ststm32/boards && \
    cp -r /tmp/Arduino_Core_STM32/* ~/.platformio/packages/framework-arduinoststm32/ || true

RUN platformio run -e genericSTM32F103CB -e esp32dev

RUN mkdir -p /artifacts/build

RUN cp -v .pio/build/genericSTM32F103CB/firmware.bin /artifacts/build/stm32_firmware.bin && \
    cp -v .pio/build/genericSTM32F103CB/firmware.elf /artifacts/build/stm32_firmware.elf

RUN cp -v .pio/build/esp32dev/firmware.bin /artifacts/build/esp32_firmware.bin && \
    cp -v .pio/build/esp32dev/firmware.elf /artifacts/build/esp32_firmware.elf

CMD ["sh", "-c", "mkdir -p /app/build && cp -rv /artifacts/build/* /app/build/ && echo 'Build files copied to host directory' && ls -la /app/build/"]
