#include "usb_manager.h"

#include <algorithm>
#include <ranges>
#include <span>
#include <vector>

#include "time_util.h"

namespace authpp {

UsbManager::UsbManager()
{
    libusb_init(&context);
}

UsbManager::~UsbManager()
{
    libusb_exit(context);
}

std::vector<UsbDevice> UsbManager::poll(std::function<bool(libusb_device_descriptor)> p)
{
    libusb_device** usbDevices;
    std::vector<UsbDevice> result;
    std::size_t usbDeviceCount = libusb_get_device_list(context, &usbDevices);

#ifdef __cpp_lib_ranges
    auto deviceSpan = std::span { usbDevices, usbDevices + usbDeviceCount };
    auto matchingDevices = deviceSpan | std::views::filter([&p](auto&& d) {
        libusb_device_descriptor desc;
        libusb_get_device_descriptor(d, &desc);
        return p(desc);
    }) | std::views::transform([](const auto& d) { return UsbDevice(d); });
    std::ranges::move(matchingDevices, std::back_inserter(result));
#else
    auto deviceSpan = std::span { usbDevices, usbDevices + usbDeviceCount };
    for (auto&& usbDevice : deviceSpan) {
        libusb_device_descriptor desc;
        libusb_get_device_descriptor(usbDevice, &desc);
        if (p(desc)) {
            result.emplace_back(usbDevice);
        }
    }
#endif

    libusb_free_device_list(usbDevices, true);
    return result;
}

std::vector<UsbDevice> UsbManager::pollUsbDevices(std::function<bool(libusb_device_descriptor)> p, long timeout)
{
    std::vector<UsbDevice> usbDevices;
    auto s = TimeUtil::getTime();
    do {
        usbDevices = poll(p);
    } while (usbDevices.empty() && (s + timeout > TimeUtil::getTime()));
    return usbDevices;
}

} // namespace authpp
