#pragma once
#include "JoyConRight.h"

class RingCon :
    public JoyConRight
{
public:
    RingCon(hid_device_info* dev_info);

    void initialize() override;

	virtual int printStats() const override;

	int getFlex() const;
	Eigen::Vector3f getGyroRing() const;

	Eigen::Vector3i getGyroRingRaw() const;

	virtual JoyData getData() const override;
	
protected:

	static uint8_t _mcu_crc8_table[256];
	static uint8_t _ringmcu_crc8_table[256];
	static uint8_t _mcu_crc8_calc(uint8_t* buffer, uint8_t size);
	static uint8_t _ringmcu_crc8_calc(uint8_t* buffer, uint8_t size);
	std::vector<uint8_t> getDefaultCommand(uint8_t command=1, bool set_rumble_base=false);

	//struct brcm_hdr {
	//	uint8_t cmd;
	//	uint8_t rumble[9];
	//};

	//struct brcm_cmd_01 {
	//	uint8_t subcmd;
	//	uint32_t offset;
	//	uint8_t size;
	//};

    bool enableMCU();
    bool enableMCUPolling();
	bool setMCUMode();
	bool getExtData();

	bool enableIMU();
	bool setExtFormatConfig();
	bool enableExtPolling();
	bool setExtConfig();

	// returns true on success
	bool communicateWithDevice(const std::vector<uint8_t>& data, const std::vector<std::pair<int, uint8_t>>& check_data, uint8_t command=1, bool set_rumble_base=false, int read_length=64);

	virtual void parseDataWithIMU(const std::vector<uint8_t>& data) override;

protected:
    bool _hasRingCon;

	uint8_t _timing_byte;

	Eigen::Vector3i _gyro_ring;
	int _flex;

    //int _ringcon;
};

