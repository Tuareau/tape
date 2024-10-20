#pragma once

#include "TapeFstreamEmulator.h"

#include <memory>

template <typename T>
class TapeEmulatorFabric
{
public:
	TapeEmulatorFabric() = delete;

	static std::shared_ptr<ITapeEmulator<T>> CreateEmulator(std::string filename);
	static std::shared_ptr<ITapeEmulator<T>> CreateEmulator(TapeSettingsPointer settings);
	static std::shared_ptr<ITapeEmulator<T>> CreateEmulator(std::string filename, TapeSettingsPointer settings);

};

template<typename T>
inline std::shared_ptr<ITapeEmulator<T>> TapeEmulatorFabric<T>::CreateEmulator(std::string filename)
{
	std::shared_ptr<ITapeEmulator<T>> emulator_ptr = std::make_shared<TapeFstreamEmulator<T>>(filename);
	return emulator_ptr;
}

template<typename T>
inline std::shared_ptr<ITapeEmulator<T>> TapeEmulatorFabric<T>::CreateEmulator(TapeSettingsPointer settings)
{
	std::shared_ptr<ITapeEmulator<T>> emulator_ptr = std::make_shared<TapeFstreamEmulator<T>>(settings);
	return emulator_ptr;
}

template<typename T>
inline std::shared_ptr<ITapeEmulator<T>> TapeEmulatorFabric<T>::CreateEmulator(std::string filename, TapeSettingsPointer settings)
{
	std::shared_ptr<ITapeEmulator<T>> emulator_ptr = std::make_shared<TapeFstreamEmulator<T>>(filename, settings);
	return emulator_ptr;
}
