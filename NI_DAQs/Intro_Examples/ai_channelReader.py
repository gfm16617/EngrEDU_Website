import nidaqmx
import numpy
from nidaqmx.stream_readers import AnalogSingleChannelReader

with nidaqmx.Task() as read_task:
    number_of_samples = 500  # change this to number of samples to acquire

    # create task and channel
    read_task.ai_channels.add_ai_voltage_chan("Dev3/ai1",
                                              max_val=5.0, min_val=0.0)

    # create stream reader object
    reader = AnalogSingleChannelReader(read_task.in_stream)

    # initialize data array
    read_array = numpy.zeros(number_of_samples, dtype=numpy.float64)

    # acquire and store in read_array
    reader.read_many_sample(
        read_array, number_of_samples_per_channel=number_of_samples,
        timeout=10.0)

    # print to console the result
    print(read_array)