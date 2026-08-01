# Texton `.clu` streamline reproduction

This Python program reads `cfd12_10_roi.clu`, reconstructs the sparse original velocity lattice, ports Texton's transient 3D streamline point-collection algorithm, and writes a static PNG.

## Run

```bash
python3 texton_streamlines.py cfd12_10_roi.clu \
  --output cfd12_10_roi_texton_paper_view.png
```

For the strict streamlines-only equivalent of Texton's **Render all** window:

```bash
python3 texton_streamlines.py cfd12_10_roi.clu \
  --output cfd12_10_roi_texton_streamlines_only.png \
  --no-shell
```

Dependencies: Python 3.10+, NumPy and Matplotlib. SciPy and scikit-image are used only for the optional faint ROI context shell.

## What is reproduced

- Texton's five-line `.clu` node serialization
- original leaf-average velocity reconstruction
- the original `FloatToInt` behaviour, including its negative-number quirk
- seed spacing of 5 voxels in x, 5 in y and 4 in z
- `CalcDeltaS3D` nearest-cell-face integration with the `+0.01` offset
- 100 positive-direction iterations
- Texton's forward-only streamline point collection
- omission of paths containing two or fewer points, as in `RenderStreamlineList`

The optional pale shell is derived from the `.clu` ROI mask to resemble the contextual ventricle silhouette in the MICCAI figure. It is not added by Texton's `RenderStreamlineList` function itself.
