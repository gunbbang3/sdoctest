Requirements
============

Same two requirements as ``docs/requirements.sdoc``, expressed as
sphinx-needs ``req`` needs instead of StrictDoc ``[REQUIREMENT]`` nodes.

.. req:: Add two integers
   :id: REQ-1

   The system shall provide a function that returns the sum of two integers.

.. req:: Reject non-numeric input
   :id: REQ-2

   The system shall raise a TypeError when the addition function is called
   with a non-numeric argument.
