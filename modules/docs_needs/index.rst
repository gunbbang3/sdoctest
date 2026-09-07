sdoctest - sphinx-needs side
============================

Cross-module index. Each module owns its own requirement, architecture,
implementation, and verification documents under
``modules/<name>/docs_needs/`` -- this file only ties them together into
one Sphinx build, the same role StrictDoc fills automatically (without an
explicit index) by discovering every ``.sdoc`` file under ``modules/``.

.. note::

   Every module's ``implementation.rst`` names its source files
   (``.h``/``.cpp``) as plain text. Unlike StrictDoc's ``@relation(...)``
   source markers, sphinx-needs does not parse those files and will not
   fail if a path becomes stale or the file is deleted -- see the
   repository's top-level ``README.md`` for the full comparison.

.. toctree::

   ../sensor_input/docs_needs/requirements
   ../sensor_input/docs_needs/architecture
   ../sensor_input/docs_needs/implementation
   ../sensor_input/docs_needs/verification
   ../signal_filter/docs_needs/requirements
   ../signal_filter/docs_needs/architecture
   ../signal_filter/docs_needs/implementation
   ../signal_filter/docs_needs/verification
   ../math_utils/docs_needs/requirements
   ../math_utils/docs_needs/architecture
   ../math_utils/docs_needs/implementation
   ../math_utils/docs_needs/verification
   ../diagnostics/docs_needs/requirements
   ../diagnostics/docs_needs/architecture
   ../diagnostics/docs_needs/implementation
   ../diagnostics/docs_needs/verification
   ../logger/docs_needs/requirements
   ../logger/docs_needs/architecture
   ../logger/docs_needs/implementation
   ../logger/docs_needs/verification
   ../controller/docs_needs/requirements
   ../controller/docs_needs/architecture
   ../controller/docs_needs/implementation
   ../controller/docs_needs/verification
   ../speed_controller/docs_needs/requirements
   ../speed_controller/docs_needs/architecture
   ../speed_controller/docs_needs/implementation
   ../speed_controller/docs_needs/verification
