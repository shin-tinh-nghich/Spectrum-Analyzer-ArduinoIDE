TaskHandle_t vLED_TaskHandle;
TaskHandle_t vISR_RotaryEncoderdHandle;
TaskHandle_t time_ScreenHandle;

void TaskSerial(void *pvParameters)
{
  (void) pvParameters;
  for (;;)
  {
    Serial.println("======== Tasks status ========");
    Serial.print("Tick count: ");
    Serial.print(xTaskGetTickCount());
    Serial.print(", Task count: ");
    Serial.print(uxTaskGetNumberOfTasks());

    Serial.println();
    Serial.println();

    // Serial task status
    Serial.print("- TASK ");
    Serial.print(pcTaskGetName(NULL)); // Get task name without handler https://www.freertos.org/a00021.html#pcTaskGetName
    Serial.print(", High Watermark: ");
    Serial.print(uxTaskGetStackHighWaterMark(NULL)); // https://www.freertos.org/uxTaskGetStackHighWaterMark.html 


    TaskHandle_t taskSerialHandle = xTaskGetCurrentTaskHandle(); // Get current task handle. https://www.freertos.org/a00021.html#xTaskGetCurrentTaskHandle
    
    Serial.println();

    Serial.print("- TASK ");
    Serial.print(pcTaskGetName(vLED_TaskHandle)); // Get task name with handler
    Serial.print(", High Watermark: ");
    Serial.print(uxTaskGetStackHighWaterMark(vLED_TaskHandle));
    Serial.println();

    Serial.print("- TASK ");
    Serial.print(pcTaskGetName(vISR_RotaryEncoderdHandle));
    Serial.print(", High Watermark: ");
    Serial.print(uxTaskGetStackHighWaterMark(vISR_RotaryEncoderdHandle));
    Serial.println();

    Serial.print("- TASK ");
    Serial.print(pcTaskGetName(time_ScreenHandle));
    Serial.print(", High Watermark: ");
    Serial.print(uxTaskGetStackHighWaterMark(time_ScreenHandle));
    Serial.println();
    
    Serial.println();
    
    vTaskDelay( 5000 / portTICK_PERIOD_MS );
  }
}
