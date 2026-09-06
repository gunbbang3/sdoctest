project = "sdoctest (sphinx-needs)"

extensions = ["sphinx_needs"]

needs_id_regex = "^[A-Z]+-[0-9]+$"

# sphinx-needs only warns on a dangling `:links:` reference by default;
# turning that warning into a build failure requires `sphinx-build -W`
# (a generic Sphinx flag, not a sphinx-needs-specific one). This is one of
# the concrete differences from StrictDoc, which errors out on a broken
# relation unconditionally. See ../README.md for the comparison.
