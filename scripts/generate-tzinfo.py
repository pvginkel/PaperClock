import pytz
import sys
import os
import datetime


def get_transitions(tz):
    transitions = []
    now = datetime.datetime.now(datetime.UTC)

    # pytz stores transition times in UTC
    # _utc_transition_times is a list of UTC datetimes when transitions occur

    if not hasattr(tz, "_utc_transition_times"):
        print(f"No transition data available for timezone '{tz}'.", file=sys.stderr)
        return transitions

    transition_times = tz._utc_transition_times
    transition_info = tz._transition_info

    # Find the last transition before now.

    start = 1

    for i in range(1, len(transition_times)):
        transition = transition_times[i]
        if transition.replace(tzinfo=datetime.timezone.utc) > now:
            break
        start = i

    # Collect all transitions from now on.

    for i in range(start, len(transition_times)):
        transition = transition_times[i]

        # Get the offset after the transition
        info = transition_info[i]
        offset_seconds = info[0].total_seconds()

        # Convert transition time to POSIX timestamp
        timestamp = int(transition.timestamp())

        transitions.append((timestamp, int(offset_seconds)))

    return transitions


transitions = get_transitions(pytz.timezone(sys.argv[2]))

with open(sys.argv[1], "w") as f:
    f.write("static DSTTransition dst_transitions[] =\n")
    f.write("{\n")

    for transition in transitions:
        f.write(f"  {{ .time = {transition[0]}, .offset = {transition[1]} }},\n")

    f.write("};\n")
