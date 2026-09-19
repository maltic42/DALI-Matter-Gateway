from pathlib import Path
import os


Import("env")

project_dir = Path(env.subst("$PROJECT_DIR"))
version = (project_dir / "VERSION").read_text().strip()
build_file = project_dir / "BUILD"

if "BUILD_NUMBER" in os.environ:
    build_number = os.environ["BUILD_NUMBER"]
else:
    try:
        build_number = str(int(build_file.read_text().strip()) + 1)
    except ValueError as error:
        raise ValueError("BUILD must contain an integer") from error
    build_file.write_text(build_number + "\n")

env.Append(
    CPPDEFINES=[
        ("APP_VERSION", '"{}"'.format(version)),
        ("APP_BUILD", '"{}"'.format(build_number)),
        ("APP_COPYRIGHT", '"Copyright (C) 2026 Malte Rudolf"'),
    ]
)