#include "ReceiverTask.h"
#include "RadioControllerBase.h"

#include <TimeMicroSeconds.h>

#if defined(USE_FREERTOS)
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#endif


ReceiverTask::ReceiverTask(uint32_t taskIntervalMicroSeconds, ReceiverBase& receiver, RadioControllerBase& radioController, ReceiverWatcher* receiverWatcher) :
    TaskBase(taskIntervalMicroSeconds),
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
#if defined(USE_FREERTOS)
    const TickType_t tickCount = xTaskGetTickCount();
#else
    const uint32_t tickCount = timeUs() / 1000;
    //const uint32_t timeMicroSeconds = timeUs();
    //_timeMicroSecondsDelta = timeMicroSeconds - _timeMicroSecondsPrevious;
    //_timeMicroSecondsPrevious = timeMicroSeconds;
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
#if defined(USE_FREERTOS)

#if defined(RECEIVER_TASK_IS_NOT_INTERRUPT_DRIVEN)
    // pdMS_TO_TICKS Converts a time in milliseconds to a time in ticks.
    const uint32_t taskIntervalTicks = pdMS_TO_TICKS(_taskIntervalMicroSeconds / 1000);
    assert(taskIntervalTicks > 0 && "ReceiverTask taskIntervalTicks is zero.");
    _previousWakeTimeTicks = xTaskGetTickCount();

    while (true) {
        // delay until the end of the next taskIntervalTicks
        vTaskDelayUntil(&_previousWakeTimeTicks, taskIntervalTicks);

        loop();
    }
#else
    while (true) {
        _receiver.WAIT_FOR_DATA_RECEIVED();
        loop();
    }
#endif // RECEIVER_TASK_IS_NOT_INTERRUPT_DRIVEN
#else
    while (true) {}
#endif // USE_FREERTOS
}

/*!
Wrapper function for ReceiverTask::Task with the correct signature to be used in xTaskCreate.
*/
[[noreturn]] void ReceiverTask::Task(void* arg)
{
    const TaskBase::parameters_t* parameters = static_cast<TaskBase::parameters_t*>(arg); // NOLINT(cppcoreguidelines-init-variables) false positive

    static_cast<ReceiverTask*>(parameters->task)->task(); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast}
}
