Controller Architecture
=========================

.. spec:: Controller module
   :id: ARCH-6
   :links: REQ-6, ARCH-1, ARCH-2, ARCH-3, ARCH-4, ARCH-5

   The ``Controller`` C++ class implements REQ-6 by composing
   SensorInput, SignalFilter, Diagnostics, Logger and MathUtils into one
   control cycle. ARCH-1, ARCH-2, ARCH-3, ARCH-4 and ARCH-5 are declared
   in their own module folders under ``modules/`` (``sensor_input/``,
   ``signal_filter/``, ``math_utils/``, ``diagnostics/``, ``logger/``);
   sphinx-needs resolves ``:links:`` project-wide regardless of which
   file a need is declared in, same as StrictDoc.
