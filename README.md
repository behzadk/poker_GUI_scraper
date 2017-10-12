# poker_GUI_scraper
A data collection tool that can be applied to any poker gui, given appropriate setup.

The current version has the image scraping techniques in place, next steps are to integrate this with a SQL database to store the collected data. The majority of this work utilises the opencv3 library.

## Current features
- Image matching for recognition of cards
- Seemingly perfect optical character recognition (OCR) of pot size and player stack
- Imperfect but reproducible OCR of player names
- Ability to observe multiple tables simultaneously, limited by machines processing power
- Limited poker math features from integration with Pokerstove library

### Immediate development
- Writing documentation

## Future development
- Integration with SQL database
- Development of analytical tools to assess player behaviour on an individual and population scale

