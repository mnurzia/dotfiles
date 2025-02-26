#!/bin/sh

notify() {
	osascript -e "display notification \"$1\" with title \"Canvas\""
}

if ! /usr/bin/env python3 canvas_reminders.py "$1" northeastern.instructure.com "Reminders"; then
	notify "An error occurred when importing Canvas reminders."
	exit 1
fi

notify "Successfully imported Canvas reminders."
