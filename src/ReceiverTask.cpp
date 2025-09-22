#include "ReceiverTask.h"
#include "RadioControllerBase.h"

#include <TimeMicroseconds.h>

#if defined(FRAMEWORK_USE_FREERTOS)
#if defined(FRAMEWORK_ESPIDF) || defined(FRAMEWORK_ARDUINO_ESP32)
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#else
#if defined(FRAMEWORK_ARDUINO_STM32)
#include <STM32FreeRTOS.h>
#endif
#include <FreeRTOS.h>
#include <task.h>
#endif
#endif


ReceiverTask::ReceiverTask(uint32_t taskIntervalMicroseconds, ReceiverBase& receiver, RadioControllerBase& radioController, ReceiverWatcher* receiverWatcher) :
    TaskBase(taskIntervalMicroseconds),
    _receiver(receiver),
    _radioController(radioController),
    _receiverWatcher(receiverWatcher)
{
}

/*!
loop() function for when not using FREERTOS
*/
void ReceiverTask::loop()
{
    // calculate _tickCountDelta to get actual deltaT value, since we may have been delayed for more than taskIntervalTicks
#if defined(FRAMEWORK_USE_FREERTOS)
    const TickType_t tickCount = xTaskGetTickCount(); // NOLINT(cppcoreguidelines-init-variables) false positive
#else
    const uint32_t tickCount = timeMs();
#endif

    _tickCountDelta = tickCount - _tickCountPrevious;
    _tickCountPrevious = tickCount;

    if (_receiver.update(_tickCountDelta)) {
        RadioControllerBase::controls_t controls; // NOLINT(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
        controls.tickCount = tickCount;
        _receiver.getStickValues(controls.throttleStick, controls.rollStick, controls.pitchStick, controls.yawStick);
        _radioController.updateControls(controls);
        // if there a watcher, then let it know there is a new packet
        if (_receiverWatcher) {
            _receiverWatcher->newReceiverPacketAvailable();
        }
    } else {
        _radioController.checkFailsafe(tickCount);
    }
}

/*!
Task function for the ReceiverTask. Sets up and runs the task loop() function.
*/
[[noreturn]] void ReceiverTask::task() // NOLINT(readability-convert-member-functions-to-static)
{
#if defined(FRAMEWORK_USE_FREERTOS)

    // BaseType_t is int, TickType_t is uint32_t
    if (_taskIntervalMicroseconds == 0) {
        // event driven scheduling
        while (true) {
            const uint32_t ticksToWait = _radioController.getTimeoutTicks();
            if (_receiver.WAIT_FOR_DATA_RECEIVED(ticksToWait) == pdPASS) {
                loop();
            } else {
                // WAIT timed out, so check failsafe
                _radioController.checkFailsafe(xTaskGetTickCount());
            }
        }
    } else {
        // time based scheduling
        const uint32_t taskIntervalTicks = _taskIntervalMicroseconds < 1000 ? 1 : pdMS_TO_TICKS(_taskIntervalMicroseconds / 1000);
        _previousWakeTimeTicks = xTaskGetTickCount();

        while (true) {
            // delay until the end of the next taskIntervalTicks
            vTaskDelayUntil(&_previousWakeTimeTicks, taskIntervalTicks);
            while (_receiver.isDataAvailable()) {
                // Read 1 byte from UART buffer and give it to the RX protocol parser
                if (_receiver.onDataReceived(_receiver.getByte())) {
                    // onDataReceived returns true once packet is complete
                    break;
                }
            }
            loop();
        }
    }
#else
    while (true) {}
#endif // FRAMEWORK_USE_FREERTOS
}

/*!
Wrapper function for ReceiverTask::Task with the correct signature to be used in xTaskCreate.
*/
[[noreturn]] void ReceiverTask::Task(void* arg)
{
    const TaskBase::parameters_t* parameters = static_cast<TaskBase::parameters_t*>(arg); // NOLINT(cppcoreguidelines-init-variables) false positive

    static_cast<ReceiverTask*>(parameters->task)->task(); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast}
}
