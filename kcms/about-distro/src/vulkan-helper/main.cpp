#include <iostream>

#include <QGuiApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVulkanInstance>
#include <QVulkanFunctions>

using namespace Qt::StringLiterals;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QVulkanInstance instance;
    if (!instance.create()) {
        qWarning("Failed to create Vulkan instance");
        return 1;
    }

    QJsonArray devicesArray;
    auto *functions = instance.functions();

    uint32_t count = 0;
    functions->vkEnumeratePhysicalDevices(instance.vkInstance(), &count, nullptr);
    if (count == 0) {
        qWarning("No Vulkan devices found");
        return 0;
    }

    QVarLengthArray<VkPhysicalDevice, 4> devices(count);
    VkResult result = functions->vkEnumeratePhysicalDevices(instance.vkInstance(), &count, devices.data());
    if (result != VK_SUCCESS) {
        qWarning("Failed to enumerate Vulkan devices: %d", result);
        return 1;
    }

    for (const auto &device : devices) {
        VkPhysicalDeviceProperties properties;
        functions->vkGetPhysicalDeviceProperties(device, &properties);

        qDebug("Physical device %d: '%s' %d.%d.%d (api %d.%d.%d vendor 0x%X device 0x%X type %d)",
               0,
               properties.deviceName,
               VK_VERSION_MAJOR(properties.driverVersion),
               VK_VERSION_MINOR(properties.driverVersion),
               VK_VERSION_PATCH(properties.driverVersion),
               VK_VERSION_MAJOR(properties.apiVersion),
               VK_VERSION_MINOR(properties.apiVersion),
               VK_VERSION_PATCH(properties.apiVersion),
               properties.vendorID,
               properties.deviceID,
               properties.deviceType);

        QJsonObject obj;
        obj["name"] = QString::fromUtf8(properties.deviceName);
        obj["type"] = static_cast<int>(properties.deviceType);
        devicesArray.append(obj);
    }

    std::cout << QJsonDocument(devicesArray).toJson(QJsonDocument::Compact).toStdString() << '\n';
    return 0;
}
