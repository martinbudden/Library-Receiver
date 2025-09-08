#include "ReceiverBase.h"
#include "ReceiverTask.h"

#include <TaskBase.h>
#include <array>
#include <cstring>

#if defined(FRAMEWORK_USE_FREERTOS)
#include <freertos/FreeRTOS.h>
#include <freertos/FreeRTOSConfig.h>
#include <freertos/task.h>
#endif


ReceiverTask* ReceiverTask::createTask(ReceiverBase& receiver, RadioControllerBase& radioController, ReceiverWatcher* receiverWatcher, uint8_t priority, uint8_t coreID)
{
    return createTask(receiver, radioController, receiverWatcher, priority, coreID, 0);
}

ReceiverTask* ReceiverTask::createTask(task_info_t& taskInfo, ReceiverBase& receiver, RadioControllerBase& radioController, ReceiverWatcher* receiverWatcher, uint8_t priority, uint8_t coreID) // NOLINT(readability-convert-member-functions-to-static)
{
    return createTask(taskInfo, receiver, radioController, receiverWatcher, priority, coreID, 0);
}

ReceiverTask* ReceiverTask::createTask(ReceiverBase& receiver, RadioControllerBase& radioController, ReceiverWatcher* receiverWatcher, uint8_t priority, uint8_t coreID, uint32_t taskIntervalMicroSeconds)
{
    task_info_t taskInfo {}; // NOLINT(cppcoreguidelines-init-variables) false positive
    return createTask(taskInfo, receiver, radioController, receiverWatcher, priority, coreID, taskIntervalMicroSeconds);
}

ReceiverTask* ReceiverTask::createTask(task_info_t& taskInfo, ReceiverBase& receiver, RadioControllerBase& radioController, ReceiverWatcher* receiverWatcher, uint8_t priority, uint8_t coreID, uint32_t taskIntervalMicroSeconds) // NOLINT(readability-convert-member-functions-to-static)
{
    static ReceiverTask receiverTask(taskIntervalMicroSeconds, receiver, radioController, receiverWatcher);

    // Note that task parameters must not be on the stack, since they are used when the task is started, which is after this function returns.
    static TaskBase::parameters_t taskParameters { // NOLINT(misc-const-correctness) false positive
        .task = &receiverTask,
    };
#if !defined(RECEIVER_TASK_STACK_DEPTH_BYTES)
    enum { RECEIVER_TASK_STACK_DEPTH_BYTES = 4096 };
#endif
    static std::array<uint8_t, RECEIVER_TASK_STACK_DEPTH_BYTES> stack;
    taskInfo = {
        .taskHandle = nullptr,
        .name = "ReceiverTask", // max length 16, including zero terminator
        .stackDepth = RECEIVER_TASK_STACK_DEPTH_BYTES,
        .stackBuffer = &stack[0],
        .priority = priority,
        .coreID = coreID,
        .taskIntervalMicroSeconds = taskIntervalMicroSeconds
    };
#if defined(FRAMEWORK_USE_FREERTOS)
    assert(std::strlen(taskInfo.name) < configMAX_TASK_NAME_LEN);
    assert(taskInfo.priority < configMAX_PRIORITIES);

    static StaticTask_t taskBuffer;
    taskInfo.taskHandle = xTaskCreateStaticPinnedToCore(
        ReceiverTask::Task,
        taskInfo.name,
        taskInfo.stackDepth / sizeof(StackType_t),
        &taskParameters,
        taskInfo.priority,
        taskInfo.stackBuffer,
        &taskBuffer,
        taskInfo.coreID
    );
    assert(taskInfo.taskHandle != nullptr && "Unable to create ReceiverTask.");
#else
    (void)taskParameters;
#endif // FRAMEWORK_USE_FREERTOS

    return &receiverTask;
}
