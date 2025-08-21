#pragma once

#include <TaskBase.h> // NOLINT(clang-diagnostic-pragma-pack)

class RadioControllerBase;
class ReceiverBase;
class ReceiverWatcher;

class ReceiverTask : public TaskBase {
public:
    ReceiverTask(uint32_t taskIntervalMicroSeconds, ReceiverBase& receiver, RadioControllerBase& RadioController, ReceiverWatcher* receiverWatcher);
public:
    static ReceiverTask* createTask(task_info_t& taskInfo, ReceiverBase& receiver, RadioControllerBase& RadioController, ReceiverWatcher* receiverWatcher, uint8_t priority, uint8_t coreID, uint32_t taskIntervalMicroSeconds);
    static ReceiverTask* createTask(ReceiverBase& receiver, RadioControllerBase& RadioController, ReceiverWatcher* receiverWatcher, uint8_t priority, uint8_t coreID, uint32_t taskIntervalMicroSeconds);
    static ReceiverTask* createTask(task_info_t& taskInfo, ReceiverBase& receiver, RadioControllerBase& RadioController, ReceiverWatcher* receiverWatcher, uint8_t priority, uint8_t coreID);
    static ReceiverTask* createTask(ReceiverBase& receiver, RadioControllerBase& RadioController, ReceiverWatcher* receiverWatcher, uint8_t priority, uint8_t coreID);
public:
    [[noreturn]] static void Task(void* arg);
    void loop();
private:
    [[noreturn]] void task();
private:
    ReceiverBase& _receiver;
    RadioControllerBase& _radioController;
    ReceiverWatcher* _receiverWatcher;
};
