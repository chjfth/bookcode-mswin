[2020-04-29] Chj:

Actually, we do not need to define `WS_TABSTOP|WS_GROUP` for any of those radio boxes in .rc file, our calling of `CheckRadioButton()` will automatically settle them for us behind the scene.

In the .rc file, I remove all WS_TABSTOP|WS_GROUP from RADIOBUTTON statements, and the radio-box grouping still works well.

This is because CheckRadioButton() sets WS_TABSTOP automatically according to our parameter input; and WS_GROUP is actually not required for those RADIOBUTTON, because each radio-group's surrounding controls has WS_GROUP as group-boundary guard.
