# The parts of hx_test_bands's picture, as HX_TEST_SCREEN_LINES takes them: a band for a row
# that passes (P) and for one that fails (F), the band that says there are more rows (M), the
# line between two bands (L) and the gap after every fifth (G)
P:= 4:03e0
F:= 4:001f
M:= 4:7fff
L:= 1:0000
G:= 3:0000

# Five rows that pass, and their gap
PASSES:= $P $L $P $L $P $L $P $L $P $G
