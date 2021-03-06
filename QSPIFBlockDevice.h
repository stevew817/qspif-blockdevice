/* mbed Microcontroller Library
 * Copyright (c) 2018 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MBED_QSPIF_BLOCK_DEVICE_H
#define MBED_QSPIF_BLOCK_DEVICE_H

#include <QSPI.h>
#include "BlockDevice.h"

namespace mbed {

/** Enum qspif standard error codes
 *
 *  @enum qspif_bd_error
 */
enum qspif_bd_error {
    QSPIF_BD_ERROR_OK                 = 0,     /*!< no error */
    QSPIF_BD_ERROR_DEVICE_ERROR       = BD_ERROR_DEVICE_ERROR, /*!< device specific error -4001 */
    QSPIF_BD_ERROR_PARSING_FAILED     = -4002, /* SFDP Parsing failed */
    QSPIF_BD_ERROR_READY_FAILED		  = -4003, /* Wait for  Mem Ready failed */
    QSPIF_BD_ERROR_WREN_FAILED        = -4004, /* Write Enable Failed */

};


/** Enum qspif polarity mode
 *
 *  @enum qspif_polarity_mode
 */
enum qspif_polarity_mode {
    QSPIF_POLARITY_MODE_0 = 0, /* CPOL=0, CPHA=0 */
    QSPIF_POLARITY_MODE_1      /* CPOL=1, CPHA=1 */
};


#define QSPIF_MAX_REGIONS	10
#define MAX_NUM_OF_ERASE_TYPES 4

class QSPIFBlockDevice : public BlockDevice {
public:
    /** Create QSPIFBlockDevice - An SFDP based Flash Block Device over QSPI bus
     *
     *  @param io0 1st IO pin used for sending/receiving data during data phase of a transaction
     *  @param io1 2nd IO pin used for sending/receiving data during data phase of a transaction
     *  @param io2 3rd IO pin used for sending/receiving data during data phase of a transaction
     *  @param io3 4th IO pin used for sending/receiving data during data phase of a transaction
     *  @param sclk QSPI Clock pin
     *  @param csel QSPI chip select pin
     *  @param clock_mode specifies the QSPI Clock Polarity mode (QSPIF_POLARITY_MODE_0/QSPIF_POLARITY_MODE_1)
     *         default value = 0
     *  @param freq Clock frequency of the QSPI bus (defaults to 40MHz)
     *
     */
    QSPIFBlockDevice(PinName io0, PinName io1, PinName io2, PinName io3, PinName sclk, PinName csel,
                     qspif_polarity_mode clock_mode,
                     int freq = 40000000);


    /** Initialize a block device
     *
     *  @return         QSPIF_BD_ERROR_OK(0) - success
     *                  QSPIF_BD_ERROR_DEVICE_ERROR - device driver transaction failed
     *                  QSPIF_BD_ERROR_READY_FAILED - Waiting for Memory ready failed or timedout
     *                  QSPIF_BD_ERROR_PARSING_FAILED - unexpected format or values in one of the SFDP tables
     */
    virtual int init();

    /** Deinitialize a block device
     *
     *  @return         QSPIF_BD_ERROR_OK(0) - success
     *                  QSPIF_BD_ERROR_DEVICE_ERROR - device driver transaction failed
     */
    virtual int deinit();

    /** Desctruct QSPIFBlockDevie
      */
    ~QSPIFBlockDevice() {deinit();}

    /** Read blocks from a block device
     *
     *  @param buffer   Buffer to write blocks to
     *  @param addr     Address of block to begin reading from
     *  @param size     Size to read in bytes, must be a multiple of read block size
     *  @return         QSPIF_BD_ERROR_OK(0) - success
     *                  QSPIF_BD_ERROR_DEVICE_ERROR - device driver transaction failed
     */
    virtual int read(void *buffer, bd_addr_t addr, bd_size_t size);

    /** Program blocks to a block device
     *
     *  The blocks must have been erased prior to being programmed
     *
     *  @param buffer   Buffer of data to write to blocks
     *  @param addr     Address of block to begin writing to
     *  @param size     Size to write in bytes, must be a multiple of program block size
     *  @return         QSPIF_BD_ERROR_OK(0) - success
     *                  QSPIF_BD_ERROR_DEVICE_ERROR - device driver transaction failed
     *                  QSPIF_BD_ERROR_READY_FAILED - Waiting for Memory ready failed or timed out
     *                  QSPIF_BD_ERROR_WREN_FAILED - Write Enable failed
     *                  QSPIF_BD_ERROR_PARSING_FAILED - unexpected format or values in one of the SFDP tables
     */
    virtual int program(const void *buffer, bd_addr_t addr, bd_size_t size);

    /** Erase blocks on a block device
     *
     *  The state of an erased block is undefined until it has been programmed
     *
     *  @param addr     Address of block to begin erasing
     *  @param size     Size to erase in bytes, must be a multiple of erase block size
     *  @return         QSPIF_BD_ERROR_OK(0) - success
     *                  QSPIF_BD_ERROR_DEVICE_ERROR - device driver transaction failed
     *                  QSPIF_BD_ERROR_READY_FAILED - Waiting for Memory ready failed or timed out
     *                  QSPIF_BD_ERROR_WREN_FAILED - Write Enable failed
     *                  QSPIF_BD_ERROR_PARSING_FAILED - unexpected format or values in one of the SFDP tables
     */
    virtual int erase(bd_addr_t addr, bd_size_t size);

    /** Get the size of a readable block
     *
     *  @return         Size of a readable block in bytes
     */
    virtual bd_size_t get_read_size() const;

    /** Get the size of a programable block
     *
     *  @return         Size of a program block size in bytes
     *  @note Must be a multiple of the read size
     */
    virtual bd_size_t get_program_size() const;

    /** Get the size of a eraseable block
     *
     *  @return         Size of a minimal erase block, common to all regions, in bytes
     *  @note Must be a multiple of the program size
     */
    virtual bd_size_t get_erase_size() const;

    /** Get the size of minimal eraseable sector size of given address
     *
     *  @param addr     Any address within block queried for erase sector size (can be any address within flash size offset)
     *  @return         Size of minimal erase sector size, in given address region, in bytes
     *  @note Must be a multiple of the program size
     */
    virtual bd_size_t get_erase_size(bd_addr_t addr);

    /** Get the total size of the underlying device
     *
     *  @return         Size of the underlying device in bytes
     */
    virtual bd_size_t size() const;

private:
    // Internal functions

    /********************************/
    /*   Calls to QSPI Driver APIs  */
    /********************************/
    // Send Program => Write command to Driver
    qspi_status_t _qspi_send_program_command(unsigned int prog_instruction, const void *buffer, bd_addr_t addr,
            bd_size_t *size);

    // Send Read command to Driver
    qspi_status_t _qspi_send_read_command(unsigned int read_instruction, void *buffer, bd_addr_t addr, bd_size_t size);

    // Send Erase Instruction using command_transfer command to Driver
    qspi_status_t _qspi_send_erase_command(unsigned int erase_instruction, bd_addr_t addr, bd_size_t size);

    // Send Generic command_transfer command to Driver
    qspi_status_t _qspi_send_general_command(unsigned int instruction_int, bd_addr_t addr, const char *tx_buffer,
            size_t tx_length, const char *rx_buffer, size_t rx_length);

    // Send Bus configure_format command to Driver
    qspi_status_t _qspi_configure_format(qspi_bus_width_t inst_width, qspi_bus_width_t address_width,
                                         qspi_address_size_t address_size, qspi_bus_width_t alt_width, qspi_alt_size_t alt_size, qspi_bus_width_t data_width,
                                         int dummy_cycles);

    // Send set_frequency command to Driver
    qspi_status_t _qsp_set_frequency(int freq);
    /********************************/


    // Soft Reset Flash Memory
    int _reset_flash_mem();

    // Configure Write Enable in Status Register
    int _set_write_enable();

    // Wait on status register until write not-in-progress
    bool _is_mem_ready();


    /* SFDP Detection and Parsing Functions */
    /****************************************/
    // Parse SFDP Headers and retrieve Basic Param and Sector Map Tables (if exist)
    int _sfdp_parse_sfdp_headers(uint32_t& basic_table_addr, size_t& basic_table_size,
                                 uint32_t& sector_map_table_addr, size_t& sector_map_table_size);

    // Parse and Detect required Basic Parameters from Table
    int _sfdp_parse_basic_param_table(uint32_t basic_table_addr, size_t basic_table_size);

    // Parse and read information required by Regions Secotr Map
    int _sfdp_parse_sector_map_table(uint32_t sector_map_table_addr, size_t sector_map_table_size);

    // Detect fastest read Bus mode supported by device
    int _sfdp_detect_best_bus_read_mode(uint8_t *basic_param_table_ptr, bool& set_quad_enable, bool& is_qpi_mode,
                                        unsigned int& read_inst);

    // Enable Quad mode if supported (1-1-4, 1-4-4, 4-4-4 bus modes)
    int _sfdp_set_quad_enabled(uint8_t *basic_param_table_ptr);

    // Enable QPI mode (4-4-4) is supported
    int _sfdp_set_qpi_enabled(uint8_t *basic_param_table_ptr);

    // Set Page size for program
    int _sfdp_detect_page_size(uint8_t *basic_param_table_ptr);

    // Detect all supported erase types
    int _sfdp_detect_erase_types_inst_and_size(uint8_t *basic_param_table_ptr, unsigned int& erase4k_inst,
            unsigned int *erase_type_inst_arr, unsigned int *erase_type_size_arr);

    /* Utilities Functions */
    /***********************/
    // Find the region to which the given offset belong to
    int _utils_find_addr_region(bd_size_t offset);

    // Iterate on all supported Erase Types of the Region to which the offset belong to.
    // Iterates from highest type to lowest
    int _utils_iterate_next_largest_erase_type(uint8_t& bitfield, int size, int offset, int boundry);

private:
    // Internal Members

    // QSPI Driver Object
    QSPI _qspi;

    bool _is_initialized;

    // Mutex is used to protect Flash device for some QSPI Driver commands that must be done sequentially with no other commands in between
    // e.g. (1)Set Write Enable, (2)Program, (3)Wait Memory Ready
    PlatformMutex _mutex;

    // Command Instructions
    unsigned int _read_instruction;
    unsigned int _prog_instruction;
    unsigned int _erase_instruction;
    unsigned int _erase4k_inst;  // Legacy 4K erase instruction (default 0x20h)

    // Up To 4 Erase Types are supported by SFDP (each with its own command Instruction and Size)
    unsigned int _erase_type_inst_arr[MAX_NUM_OF_ERASE_TYPES];
    unsigned int _erase_type_size_arr[MAX_NUM_OF_ERASE_TYPES];

    // Sector Regions Map
    int _regions_count; //number of regions
    int _region_size_bytes[QSPIF_MAX_REGIONS]; //regions size in bytes
    bd_size_t _region_high_boundary[QSPIF_MAX_REGIONS]; //region high address offset boundary
    //Each Region can support a bit combination of any of the 4 Erase Types
    uint8_t _region_erase_types_bitfield[QSPIF_MAX_REGIONS];
    int _min_common_erase_size; // minimal common erase size for all regions (0 if none exists)

    int _page_size_bytes; // Page size - 256 Bytes default
    bd_size_t _device_size_bytes;

    // Bus speed configuration
    qspi_bus_width_t _inst_width; //Bus width for Instruction phase
    qspi_bus_width_t _address_width; //Bus width for Address phase
    qspi_address_size_t _address_size; // number of bytes for address
    qspi_bus_width_t _data_width; //Bus width for Data phase
    int _dummy_and_mode_cycles; // Number of Dummy and Mode Bits required by Current Bus Mode


};

} //namespace mbed
#endif
