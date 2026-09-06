Implementation
==============

One ``impl`` need per module, linked back to its architecture element.

.. note::

   Unlike StrictDoc's ``@relation(...)`` source markers on the actual
   ``.h``/``.cpp`` files, these file paths are plain text: sphinx-needs
   does not parse ``src/`` and will not fail if a path becomes stale or
   the file is deleted. See ``../README.md`` for the full comparison.

.. impl:: SensorInput implementation
   :id: IMPL-1
   :links: ARCH-1

   Implemented in ``src/sensor_input/sensor_input.h`` and
   ``src/sensor_input/sensor_input.cpp``.

.. impl:: SignalFilter implementation
   :id: IMPL-2
   :links: ARCH-2

   Implemented in ``src/signal_filter/signal_filter.h`` and
   ``src/signal_filter/signal_filter.cpp``.

.. impl:: MathUtils implementation
   :id: IMPL-3
   :links: ARCH-3

   Implemented in ``src/math_utils/math_utils.h`` and
   ``src/math_utils/math_utils.cpp``.

.. impl:: Diagnostics implementation
   :id: IMPL-4
   :links: ARCH-4

   Implemented in ``src/diagnostics/diagnostics.h`` and
   ``src/diagnostics/diagnostics.cpp``.

.. impl:: Logger implementation
   :id: IMPL-5
   :links: ARCH-5

   Implemented in ``src/logger/logger.h`` and ``src/logger/logger.cpp``.

.. impl:: Controller implementation
   :id: IMPL-6
   :links: ARCH-6

   Implemented in ``src/controller/controller.h`` and
   ``src/controller/controller.cpp``.
