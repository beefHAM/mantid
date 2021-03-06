.. algorithm::

.. summary::

.. relatedalgorithms::

.. properties::

Description
-----------

Given a PeaksWorkspace with a :ref:`UB matrix <Lattice>` corresponding to a Niggli
reduced cell, this algorithm will allow the user to select a
conventional cell with a specified cell type and centering. If the apply
flag is not set, the information about the selected cell will just be
displayed. If the apply flag is set, the :ref:`UB matrix <Lattice>` associated with the
sample in the PeaksWorkspace will be updated to a :ref:`UB matrix <Lattice>` corresponding to
the selected cell AND the peaks will be re-indexed using the new :ref:`UB matrix <Lattice>`.
The output transformation matrix :math:`M` will change :math:`UB` to :math:`UBM^{-1}`
and map each :math:`(HKL)` vector to :math:`M(HKL)`. It can be further
used by the :ref:`TransformHKL <algm-TransformHKL>` algorithm.
NOTE: The possible conventional cells, together with the corresponding errors in the cell
scalars can be seen by running the :ref:`ShowPossibleCells <algm-ShowPossibleCells>`
algorithm, provided the stored :ref:`UB matrix <Lattice>` corresponds to a Niggli reduced cell.

This algorithm is based on the paper: Alan D. Mighell, *Lattice
Symmetry and Identification—The Fundamental Role of Reduced Cells in
Materials Characterization.* Journal of research of the National
Institute of Standards and Technology **106.6** (2001): 983, available
from: `nvlpubs <http://nvlpubs.nist.gov/nistpubs/jres/106/6/j66mig.pdf>`_.

Usage
-----

**Example:**

.. testcode:: ExSelectCellOfType

   ws=LoadIsawPeaks("TOPAZ_3007.peaks")
   FindUBUsingFFT(ws,MinD=8.0,MaxD=13.0)
   print("Lattice before SelectCellOfType:")
   lattice = ws.sample().getOrientedLattice()
   print(" ".join("{:.9f}".format(x) for x in [lattice.a(), lattice.b(), lattice.c(),
                   	                        lattice.alpha(), lattice.beta(), lattice.gamma()]))
   SelectCellOfType(PeaksWorkspace=ws, CellType='Monoclinic', Centering='C', Apply=True)
   print("\nLattice after SelectCellOfType:")
   lattice = ws.sample().getOrientedLattice()
   print(" ".join("{:.9f}".format(x) for x in [lattice.a(), lattice.b(), lattice.c(),
                   	                        lattice.alpha(), lattice.beta(), lattice.gamma()]))


Output:

.. testoutput:: ExSelectCellOfType

    Lattice before SelectCellOfType:
    8.605818643 11.935925461 11.941812766 107.429088323 98.752912466 98.951193475

    Lattice after SelectCellOfType:
    14.131051152 19.247332564 8.605818643 89.881170675 105.071333770 89.970386662


.. categories::

.. sourcelink::
