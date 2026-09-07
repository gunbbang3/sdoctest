project = "sdoctest (sphinx-needs)"

extensions = ["sphinx_needs"]

needs_id_regex = "^[A-Z]+-[0-9]+$"

# Requirement/architecture/implementation/verification docs live inside
# each module's own folder (modules/<name>/docs_needs/*.rst), not here.
# Sphinx requires the root document to be within the source directory
# passed to sphinx-build, so this folder's `index.rst` is that root
# document, and `bazel run //:sphinx-build` / the traceability test pass
# `modules/` (this folder's parent) as the source directory with `-c` (a
# generic Sphinx flag) pointing the config directory back at this folder.
root_doc = "docs_needs/index"

# sphinx-needs only warns on a dangling `:links:` reference by default;
# turning that warning into a build failure requires `sphinx-build -W`
# (a generic Sphinx flag, not a sphinx-needs-specific one). This is one of
# the concrete differences from StrictDoc, which errors out on a broken
# relation unconditionally. See ../../README.md for the comparison.
