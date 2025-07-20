# Customizing the Dongle Display

This document describes how to adapt the LVGL based display widgets used by the dongle display shield. All widgets are found in `boards/shields/dongle_display/widgets` and can be configured via Kconfig and by editing the C source files.

## Kconfig Options

The shield provides two main options in `boards/shields/dongle_display/Kconfig.defconfig`:

| Option | Description |
|-------|------------|
| `CONFIG_ZMK_DONGLE_DISPLAY_DONGLE_BATTERY` | Show the dongle battery level alongside the peripheral batteries. |
| `CONFIG_ZMK_DONGLE_DISPLAY_MAC_MODIFIERS` | Use macOS modifier icons instead of Windows symbols. |

Additional defaults such as memory pool size and color depth are also defined in this file and can be adjusted for different displays.

## `custom_status_screen.c`

Creates the screen and positions all widgets.

- Global font and spacing are set when initializing `global_style`.
- Each widget is aligned with `lv_obj_align` or `lv_obj_align_to`. Change these parameters to move widgets around.

## `battery_status.c`

Displays battery levels of the dongle and connected peripherals.

| Area | Customization |
|------|---------------|
| `SOURCE_OFFSET` macro | Enables an extra slot when dongle battery display is configured. |
| `draw_battery()` | Adjust fill thresholds or colors for the battery icon. |
| Widget initialization | `lv_obj_align` calls set the position of each battery entry. |

## `bongo_cat.c` / `bongo_cat_images.c`

Shows an animated "Bongo Cat" that reacts to typing speed.

| Constant | Purpose |
|---------|---------|
| `ANIMATION_SPEED_IDLE` | Frame duration when idle (ms). |
| `ANIMATION_SPEED_SLOW` | Frame duration for slow typing. |
| `ANIMATION_SPEED_MID` | Frame duration for moderate typing. |
| `ANIMATION_SPEED_FAST` | Frame duration for fast typing. |

The thresholds for switching animations are in `set_animation()`. Replace the image arrays in `bongo_cat_images.c` to use your own frames.

## `modifiers.c` / `modifiers_sym.c`

Indicates pressed modifier keys.

| Item | Notes |
|------|------|
| `SIZE_SYMBOLS` in `modifiers.h` | Controls icon size (14×14 px by default). |
| `modifier_symbols[]` array | Determines icon order and which modifiers are displayed. Different sets are used for Windows and macOS. |
| `move_object_y()` | Animation for moving symbols when a modifier activates; adjust duration or path to change the effect. |

Custom icon bitmaps live in `modifiers_sym.c` and can be replaced.

## `layer_status.c`

Shows the highest active layer.

- `lv_obj_set_width` and `lv_label_set_long_mode` define label width and clipping mode.
- `set_layer_symbol()` sets the printed text, so you can change prefixes or text formatting here.

## `hid_indicators.c`

Displays Caps Lock, Num Lock and Scroll Lock status.

- Edit `LED_NLCK`, `LED_CLCK` and `LED_SLCK` masks if you wish to react to different HID bits.
- Modify `set_hid_indicators()` to change the label text that appears when locks are active.

## `output_status.c` / `output_status_sym.c`

Shows the active output (USB or BLE), BLE profile and connection status.

| Function/Area | Customization |
|---------------|--------------|
| `move_object_x()` and `change_size_object()` | Animation helpers for moving the selection line. Adjust duration or animation path here. |
| `zmk_widget_output_status_init()` | `lv_obj_align` calls position the USB/BLE symbols, profile number and status icons. |
| Symbol tables in `output_status_sym.c` | Replace or add icons for USB, Bluetooth and profile numbers. |

---

By adjusting these constants and alignment calls you can tailor fonts, animations and layout to match your preferences or a different display size. Rebuild the firmware after changing Kconfig options or any of the widget sources.
