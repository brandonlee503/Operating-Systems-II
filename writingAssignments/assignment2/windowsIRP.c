NTSTATUS driverIRPDispatcher(PDEVICE_OBJECT DeviceObject, PIRP Irp);

NTSTATUS DriverEntry (IN PDRIVER_OBJECT pDriverObject, IN PUNICODE_STRING strRegistryPath ) {
    pDriverObject->DriverUnload = driverCleanupFunction;
    return STATUS_SUCCESS;
}
