# Acuity Serial Number Generator
A flexible, standalone module that provides atomic, auto-incrementing serial number generation.

This module allows you to generate sequential serial numbers and:
* **Add them to Content Titles:** Automatically populate the Title field of specific content types.
* **Add them to Text Fields:** Define custom *Text (short)* fields and apply the serial number via the "Auto serial number" widget.

# About **Acuity**
Acuity is a branding 

**How it works:**
To prevent duplicates (race conditions), the serial number field shows the *next available* serial number on the data entry screen as a **read-only** field. This displayed number is provisional (an indication of what is available when the page loads); the module will lock and assign the next available atomic number when the content is actually **saved**.

## Features

- Atomic, gap-minimising serial generation using Backdrop's Lock API.
- Configurable prefix, suffix, zero-padding, and next value.
- No gaps unless save operation fails (should be very rare).
- Lightweight and usable across multiple modules or systems.

## Initial version

This is a beta release for code review and testing.

### Requirements:

- Backdrop CMS 1.x
- PHP 8+ (While the code may technically function with PHP 7.4 at this time, we strictly require PHP 8.0+ and will not address issues related to older PHP versions.)

## Installation:

Install this module using the official Backdrop CMS instructions at https://docs.backdropcms.org/documentation/extend-with-modules

Enable the module.


## Documentation:

Visit the admin page at /admin/config/acuity-utils/acuity_sn

Configuration
 .. Acuity utils
  ... Serial number settings

Set options for: -

 - Machine name: A machine-readable field name must consist of lowercase letters, numbers and underscores, but it must start with a character. i.e. "invoice_number", "po_number", "widget_serial".
 - Label: This is the human-readable text and field label used on forms and when selecting the widget from the Content Type field settings.
 - Prefix: Automatically added to the beginning of the serial number, i.e. "*Inv*00125".
 - Suffix: Automatically appended to the end of the serial number, i.e. "Inv00125*pending*".
 - Padding: Zero padding adds a 0 to the left of the serial number, i.e. 4 = 0001, 5 = 00001.
 - Next: Next serial number to use. This is also used for the starting number.  This should be numeric with no padding, i.e. "1".

To add a serial number to a Content type: -
 - add/modify a *Text (short)* fields widget to *Auto serial number*.
 - From the "Serial number group" selection, choose one of the "Serial number groups" created from the settings page.  This will show as the human-readable name and the machine-readable name, i.e. *Invoice Number (invoie)*, *Job Number (job_no)*.
 - The Text Processor should be "Plain text"

### Important Behaviours

- **Permanent Assignment:** Serial numbers are designed to be unique and strictly sequential (e.g., for Invoices, Purchase Orders, Batch Numbers). Once a number is assigned to a node, it is permanent. Deleting a node will **not** release that serial number for reuse; the counter will continue from the "Next number" defined in the settings.
- **Settings are not Retroactive:** Changing the settings for a Serial Group (e.g., altering the Prefix or Suffix) will **not** affect existing content. The serial number is saved as plain text in the database. The module only inserts a serial number if the field is empty; if a field already contains data, it will remain unchanged.

 ### Existing Content

This module assigns numbers to new content going forward. It does not retroactively generate numbers for existing nodes.

**Important:** If you are upgrading an existing content type where you previously manually entered serial numbers, changing the widget to "Auto serial number" will make those fields read-only. You will not be able to edit the existing values.

**Recommendation:** Configure the Serial Number Group's **Next number** to be greater than your highest existing manual serial number to avoid conflicts.

**API**
***NOTE: The API is a work-in-progress... Please contact the developer before using the API in production.***

You can request a serial number programmatically using the following function:

```php
$serial = acuity_sn_generate_number('repair');
```
Implement `hook_acuity_sn_info()` in your module to define serial groups.

## Issues:

Bugs and Feature requests should be reported in the Issue Queue: https://github.com/backdrop-contrib/acuity_sn/issues

## About our "Acuity" Brand
"Acuity" is the namespace used by Albany Computer Services to organise our collection of modules and utilities. We group these tools under a unified brand to streamline installation and make it easier for site maintainers to identify our suite of developed solutions. While part of this larger family, this module is a standalone utility.

## Current Maintainer(s):
- Steve Moorhouse (albanycomputers) (https://github.com/albanycomputers)
- Seeking additional maintainers and contributors.

## Credits:
- Steve Moorhouse - Zulip (DrAlbany)
- Google Gemini 3.0 assisted with the coding of this module.

## Sponsorship:
- Albany Computer Services (https://www.albany-computers.co.uk)
- Albany Web Design (https://www.albanywebdesign.co.uk)
- Albany Hosting (https://www.albany-hosting.co.uk)

## License
This project is GPL v2 software. See the LICENSE.txt file in this directory for complete text.
