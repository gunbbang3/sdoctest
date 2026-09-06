Requirements
============

Same seven requirements as ``docs/requirements.sdoc``, expressed as
sphinx-needs ``req`` needs instead of StrictDoc ``[REQUIREMENT]`` nodes.

.. req:: Provide raw sensor readings
   :id: REQ-1

   The system shall provide access to raw, unfiltered readings from a
   sensor input source, indexed by sample number.

.. req:: Smooth noisy sensor signal
   :id: REQ-2

   The system shall smooth a noisy input stream using a moving-average
   filter over a configurable window size.

.. req:: Provide arithmetic utility functions
   :id: REQ-3

   The system shall provide reusable arithmetic utility functions,
   including addition and clamping a value into a closed interval.

.. req:: Detect out-of-range signals
   :id: REQ-4

   The system shall detect when a signal value falls outside a configured
   valid range and report a fault code.

.. req:: Log events and faults with severity
   :id: REQ-5

   The system shall record log entries tagged with a severity level
   (info, warning, or error) so they can be reviewed later.

.. req:: Integrate acquisition, filtering, diagnostics, and logging
   :id: REQ-6

   The system shall integrate sensor acquisition, signal filtering, range
   diagnostics, and logging into a single control cycle that processes
   every available sample.

.. req:: Compute an actuator command with a PI control law
   :id: REQ-7

   The system shall compute an actuator command from an error signal
   using a proportional-integral (PI) control law, with the output
   saturated to a configured range.
