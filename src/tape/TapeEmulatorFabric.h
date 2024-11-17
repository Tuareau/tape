#pragma once

#include "TapeFstreamEmulator.h"

#include <memory>

template <typename T>
class TapeEmulatorFabric
{
public:
	TapeEmulatorFabric() = delete;

	static std::shared_ptr<ITapeEmulator<T>> CreateEmulator(std::filesystem::path filepath);
	static std::shared_ptr<ITapeEmulator<T>> CreateEmulator(TapeSettingsPointer settings);
	static std::shared_ptr<ITapeEmulator<T>> CreateEmulator(std::filesystem::path filepath, TapeSettingsPointer settings);
};

template<typename T>
inline std::shared_ptr<ITapeEmulator<T>> TapeEmulatorFabric<T>::CreateEmulator(std::filesystem::path filepath)
{
	std::shared_ptr<ITapeEmulator<T>> emulator_ptr = std::make_shared<TapeFstreamEmulator<T>>(filepath);
	return emulator_ptr;
}

template<typename T>
inline std::shared_ptr<ITapeEmulator<T>> TapeEmulatorFabric<T>::CreateEmulator(TapeSettingsPointer settings)
{
	std::shared_ptr<ITapeEmulator<T>> emulator_ptr = std::make_shared<TapeFstreamEmulator<T>>(settings);
	return emulator_ptr;
}

template<typename T>
inline std::shared_ptr<ITapeEmulator<T>> TapeEmulatorFabric<T>::CreateEmulator(std::filesystem::path filepath, TapeSettingsPointer settings)
{
	std::shared_ptr<ITapeEmulator<T>> emulator_ptr = std::make_shared<TapeFstreamEmulator<T>>(filepath, settings);
	return emulator_ptr;
}
