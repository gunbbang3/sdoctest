SpeedController Architecture
==============================

.. spec:: SpeedController module (Simulink MBD generated)
   :id: ARCH-7
   :links: REQ-7

   The ``speed_controller`` C module implements REQ-7. It is generated
   from a Simulink model (see ``../models/speed_controller.md``, in this
   same module folder) with Simulink Embedded Coder and must not be
   hand-edited. sphinx-needs has no doc-side equivalent of StrictDoc's
   ``TYPE: File`` relation, so unlike this module's
   ``../docs/architecture.sdoc`` (ARCH-7), this need cannot itself
   assert that ``../src/speed_controller.c`` exists -- see
   ``../../../README.md`` for the comparison.
