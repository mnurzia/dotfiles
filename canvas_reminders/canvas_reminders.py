import argparse
import urllib.request
import sys
import json
import datetime
import subprocess
import re


if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("api_key", type=str, help="Canvas API key")
    ap.add_argument("canvas_url", type=str, help="Canvas URL")
    ap.add_argument("reminder_list", type=str, help="Reminder List")
    args = ap.parse_args()

    def request(stem):
        req = urllib.request.Request(f"https://{args.canvas_url}/{stem}")
        req.add_header("Authorization", "Bearer " + args.api_key)
        with urllib.request.urlopen(req) as response:
            return json.load(response)

    reminders = dict(
        map(
            lambda r: (int(r[1]["notes"][len("canvas id: ") :]), (r[0], r[1])),
            filter(
                lambda r: r[1].get("notes", "").startswith("canvas id: "),
                enumerate(
                    json.loads(
                        subprocess.run(
                            [
                                "reminders",
                                "show",
                                args.reminder_list,
                                "--format",
                                "json",
                            ],
                            stdout=subprocess.PIPE,
                        ).stdout
                    )
                ),
            ),
        )
    )

    courses_json = request("api/v1/users/self/favorites/courses")
    courses = {c["id"]: c["course_code"] for c in courses_json}
    for course_id, course_code in courses.items():
        abbrev_course_code = re.match(r".*?(\d+)[.].*", course_code)
        assert abbrev_course_code is not None
        abbrev_course_code = abbrev_course_code.group(1)
        assignments = request(f"api/v1/courses/{course_id}/assignments?bucket=future")
        for assignment in assignments:
            if assignment["due_at"] is None:
                continue
            name = abbrev_course_code + " " + assignment["name"]
            id = assignment["id"]
            new_due_date = datetime.datetime.fromisoformat(assignment["due_at"])
            create_new_reminder = True
            if id in reminders:
                idx, reminder = reminders[id]
                old_due_date = datetime.datetime.fromisoformat(reminder["dueDate"])
                old_name = reminder["title"]
                if old_due_date != new_due_date or old_name != name:
                    subprocess.run(
                        ["reminders", "delete", args.reminder_list, str(idx)]
                    )
                    reminders = {
                        id: (old_idx if old_idx < idx else old_idx - 1, r)
                        for (id, (old_idx, r)) in reminders.items()
                    }
                    create_new_reminder = True
                else:
                    create_new_reminder = False
            if create_new_reminder:
                subprocess.run(
                    [
                        "reminders",
                        "add",
                        args.reminder_list,
                        name,
                        "--due-date",
                        new_due_date.isoformat(),
                        "--notes",
                        f"canvas id: {id}",
                    ]
                )
