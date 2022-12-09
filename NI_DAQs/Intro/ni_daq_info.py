import nidaqmx
from nidaqmx.system import System


def get_DAQ_Info():
    #  Get local system object
    local_system = System.local()

    #  Get version info
    driver_version = local_system.driver_version
    print("DAQmx {0}.{1}.{2}".format(driver_version.major_version, driver_version.minor_version,
                    driver_version.update_version))

    #  Get Device Names
    for device in local_system.devices:
        print("Device Name: {0}, Product Category: {1}, Product Type: {2}".format(
            device.name, device.product_category, device.product_type
        ))


if __name__ == "__main__":
    get_DAQ_Info()
