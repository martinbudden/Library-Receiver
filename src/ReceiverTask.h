#pragma once

#include <TaskBase.h> // NOLINT(clang-diagnostic-pragma-pack)

class RadioControllerBase;
class ReceiverBase;
class ReceiverWatcher;

class ReceiverTask : public TaskBase {
public:
    ReceiverTask(uint32_t taskIntervalMicroseconds, RadioControllerBase& RadioController, ReceiverWatcher* receiverWatcher);
public:
    static ReceiverTask* createTask(task_info_t& taskInfo, RadioControllerBase& RadioController, ReceiverWatcher* receiverWatcher, uint8_t priority, uint32_t core, uint32_t taskIntervalMicroseconds);
    static ReceiverTask* createTask(RadioControllerBase& RadioController, ReceiverWatcher* receiverWatcher, uint8_t priority, uint32_t core, uint32_t taskIntervalMicroseconds);
    static ReceiverTask* createTask(task_info_t& taskInfo, RadioControllerBase& RadioController, ReceiverWatcher* receiverWatcher, uint8_t priority, uint32_t core);
    static ReceiverTask* createTask(RadioControllerBase& RadioController, ReceiverWatcher* receiverWatcher, uint8_t priority, uint32_t core);
public:
    [[noreturn]] static void Task(void* arg);
    void loop();
private:
    [[noreturn]] void task();
private:
    RadioControllerBase& _radioController;
    ReceiverBase& _receiver;
    ReceiverWatcher* _receiverWatcher;
};
