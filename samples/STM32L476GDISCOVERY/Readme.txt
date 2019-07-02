1. [Introduction]
   This project is an example of nuklear eve on STM32L476GDISCOVERY board
   The EVE platform is BT815

2. [Connect hardware]
    ______________________________________
   |   STM32L476GDISCOVERY|    BT815       |
   | ---------------------|----------------|
   | Function    | Port   |    wire to     |
   | ------------|--------|----------------|
   | (PDN)       | PB2    | -----> PD#     |
   | (CS)        | PE8    | -----> CS#     |
   |             | GND    | -----> GND     |
   | (SPI1_SCK ) | PE13   | -----> SCK     |
   | (SPI1_MISO) | PE14   | -----> MISO    |
   | (SPI1_MOSI) | PE15   | -----> MOSI    |
   |______________________|________________|
    
   - Connect mini USB port from STM32L476GDISCOVERY to USB port on the PC
   
3. [How to build and run]
    - Open this project with STM32CubeIDE software
    - Connect hardware like [2]
    - Build (Ctrl + B)
    - Run: Run -> Debug configuration -> STM32 MCU Debugging -> Add new launch configuration -> Debug
    
    
    
                                  END
                                  