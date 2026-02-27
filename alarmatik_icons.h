#pragma once

/* minimal stubs for the icon definitions referenced by Application.c */

/* icon data is opaque to the host build; use a dummy type */
typedef struct {
    int dummy;
} AlarmatikIcon;

/* declare the icon used in code */
extern const AlarmatikIcon I_IR_On;
