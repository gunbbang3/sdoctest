Architecture
============

Mirrors ``docs/architecture.sdoc``. Each parent link is expressed with
sphinx-needs' ``:links:`` option instead of StrictDoc's
``RELATIONS: TYPE: Parent``.

.. spec:: SensorInput module
   :id: ARCH-1
   :links: REQ-1

   The ``SensorInput`` C++ class implements REQ-1 by exposing raw sensor
   samples by index.

.. spec:: SignalFilter module
   :id: ARCH-2
   :links: REQ-2

   The ``MovingAverageFilter`` C++ class implements REQ-2 by maintaining a
   sliding window over the most recent samples.

.. spec:: MathUtils module
   :id: ARCH-3
   :links: REQ-3

   The ``math_utils`` C++ functions (``Add``, ``Clamp``) implement REQ-3.

.. spec:: Diagnostics module
   :id: ARCH-4
   :links: REQ-4

   The ``RangeDiagnostics`` C++ class implements REQ-4 by checking a value
   against a configured [lo, hi] range.

.. spec:: Logger module
   :id: ARCH-5
   :links: REQ-5

   The ``Logger`` C++ class implements REQ-5 by recording severity-tagged
   entries.

.. spec:: Controller module
   :id: ARCH-6
   :links: REQ-6, ARCH-1, ARCH-2, ARCH-3, ARCH-4, ARCH-5

   The ``Controller`` C++ class implements REQ-6 by composing SensorInput,
   SignalFilter, Diagnostics, Logger and MathUtils into one control cycle.
