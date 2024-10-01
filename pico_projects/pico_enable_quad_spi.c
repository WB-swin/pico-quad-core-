#include "pico/stdlib.h"
#include "hardware/spi.h"

#define CS_PIN 5          // Define the Chip Select (CS) pin
#define SPI_PORT spi0     // Use SPI0 port

// Define the command opcodes for the flash memory
#define WRITE_ENABLE 0x06            // Write Enable (06h)
#define WRITE_STATUS_REG2 0x31       // Write Status Register-2 (31h)
#define READ_STATUS_REG2 0x35        // Read Status Register-2 (35h)

// Function to initialize the SPI and GPIO pins
void init_spi() {
    // Initialize SPI with a lower frequency (e.g., 1MHz) for configuration
    spi_init(SPI_PORT, 1000 * 1000);

    // Configure the SPI pins
    gpio_set_function(2, GPIO_FUNC_SPI);  // GPIO2 for SCK (SPI Clock)
    gpio_set_function(3, GPIO_FUNC_SPI);  // GPIO3 for MOSI (Master Out Slave In)
    gpio_set_function(4, GPIO_FUNC_SPI);  // GPIO4 for MISO (Master In Slave Out)
    gpio_set_function(CS_PIN, GPIO_OUT);  // CS pin for Chip Select

    // Set the Chip Select pin high (deselect flash memory)
    gpio_put(CS_PIN, 1);
}

// Function to enable writing on the flash memory (required before modifying status registers)
void write_enable() {
    uint8_t cmd = WRITE_ENABLE;

    // Select the flash memory
    gpio_put(CS_PIN, 0);
    spi_write_blocking(SPI_PORT, &cmd, 1);  // Send Write Enable (06h) command
    gpio_put(CS_PIN, 1);                    // Deselect the flash memory
}

// Function to write to Status Register-2 and set the QE bit
void set_qe_bit() {
    uint8_t status_reg2;

    // Enable writing on the flash memory
    write_enable();

    // Read the current value of Status Register-2
    uint8_t read_cmd = READ_STATUS_REG2;
    gpio_put(CS_PIN, 0);                                 // Select the flash memory
    spi_write_blocking(SPI_PORT, &read_cmd, 1);          // Send Read Status Register-2 (35h) command
    spi_read_blocking(SPI_PORT, 0, &status_reg2, 1);     // Read one byte from Status Register-2
    gpio_put(CS_PIN, 1);                                 // Deselect the flash memory

    // Set the QE bit (bit 1) if it's not already set
    if (!(status_reg2 & 0x02)) {  // Check if the QE bit is already set
        write_enable();           // Enable writing again before modifying status register

        // Set the QE bit (bit 1 of Status Register-2)
        uint8_t write_cmd[2] = {WRITE_STATUS_REG2, status_reg2 | 0x02};  // Set QE bit to 1
        gpio_put(CS_PIN, 0);                                             // Select the flash memory
        spi_write_blocking(SPI_PORT, write_cmd, 2);                      // Send Write Status Register-2 command and new value
        gpio_put(CS_PIN, 1);                                             // Deselect the flash memory
    }
}

// Function to read and verify the QE bit
void verify_qe_bit() {
    uint8_t status_reg2;

    // Read the current value of Status Register-2
    uint8_t read_cmd = READ_STATUS_REG2;
    gpio_put(CS_PIN, 0);                                 // Select the flash memory
    spi_write_blocking(SPI_PORT, &read_cmd, 1);          // Send Read Status Register-2 (35h) command
    spi_read_blocking(SPI_PORT, 0, &status_reg2, 1);     // Read one byte from Status Register-2
    gpio_put(CS_PIN, 1);                                 // Deselect the flash memory

    // Check if QE bit is set
    if (status_reg2 & 0x02) {
        printf("Quad Enable (QE) bit is set successfully.\n");
    } else {
        printf("Failed to set the Quad Enable (QE) bit.\n");
    }
}

int main() {
    stdio_init_all();    // Initialize standard I/O (for debugging purposes)
    init_spi();          // Initialize SPI and GPIO pins

    set_qe_bit();        // Set the Quad Enable (QE) bit in Status Register-2
    verify_qe_bit();     // Verify that the QE bit is set

    while (true) {
        tight_loop_contents();  // Keep the program running
    }

    return 0;
}
