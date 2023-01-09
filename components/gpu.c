#include <X11/Xlib.h>
#include "NVCtrl/NVCtrl.h"
#include "NVCtrl/NVCtrlLib.h"
#include "../util.h"

// search NVCtrl.h for the needed attribute

const char *nvidia_temp(const char *gpu_id)
{
	Display *dpy;
	if (!(dpy = XOpenDisplay(NULL)))
	{
		warn("XOpenDisplay: Failed to open display");
		return NULL;
	}

	int target_id = (*gpu_id) - '0';
	int value = -1;
	XNVCTRLQueryTargetAttribute(dpy, NV_CTRL_TARGET_TYPE_GPU, target_id, 0, NV_CTRL_GPU_CORE_TEMPERATURE, &value);
	XCloseDisplay(dpy);
	return bprintf("%d", value);
}

const char *nvidia_perc(const char *gpu_id)
{
	Display *dpy;
	if (!(dpy = XOpenDisplay(NULL)))
	{
		warn("XOpenDisplay: Failed to open display");
		return NULL;
	}

	int target_id = (*gpu_id) - '0';
	char *value = NULL; // returns (graphics=0, memory=0, video=0, PCIe=0)
	XNVCTRLQueryTargetStringAttribute(dpy, NV_CTRL_TARGET_TYPE_GPU, target_id, 0, NV_CTRL_STRING_GPU_UTILIZATION, &value);
	XCloseDisplay(dpy);
	if (value == NULL)
	{
		return "-1";
	}
	// get the precentage after "graphics="
	int start = 9, end = 9;
	while (value[end++] == ',')
		;
	value[end] = '\0';
	return &value[start];
}