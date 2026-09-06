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
        include_doc_paths=[
            "/docs/",
        ],
        include_source_paths=[
            "/src/**",
            "/tests/**",
        ],
    )
