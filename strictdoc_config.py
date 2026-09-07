from strictdoc.core.project_config import ProjectConfig


def create_config() -> ProjectConfig:
    return ProjectConfig(
        project_title="sdoctest",
        project_features=[
            "TABLE_SCREEN",
            "TRACEABILITY_SCREEN",
            "DEEP_TRACEABILITY_SCREEN",
            "TRACEABILITY_MATRIX_SCREEN",
            "REQUIREMENT_TO_SOURCE_TRACEABILITY",
        ],
        # Every module owns its own docs/ (StrictDoc) and src/, tests/
        # (source) under modules/<name>/ -- StrictDoc discovers .sdoc
        # files and @relation(...) source markers recursively, so a
        # single root path covers all modules regardless of folder depth.
        include_doc_paths=[
            "/modules/",
        ],
        include_source_paths=[
            "/modules/**",
        ],
    )
