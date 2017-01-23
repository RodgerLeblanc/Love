Love (intermediate)
-----
BlackBerry 10 app to share Love messages anonymously (with automatic (and manual) offensive language screening).

Uses : 
- Microsoft Azure Cognitive Services (language detection and profanity screening)
- IBM Cloudant database-as-a-service

To use this app, you'll need to provide your own API keys for both Azure and Cloudant, each of them offers a free version that allows limited use of their APIs, which should be enough for development. Rename `\LoveService\src\Utils\ApiKey_SAMPLE.hpp` to `\LoveService\src\Utils\ApiKey.hpp` and update the API keys in the file.

To manually dismiss a user who would have posted offensive language, add a `banned` key with any dumb value to the corresponding document in Cloudant.


LICENSE
--------------
The MIT License (MIT)

Copyright (c) 2017 Roger Leblanc

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
