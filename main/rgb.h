/*
   Copyright 2022 Adam McDaniel

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef RGB_H
#define RGB_H

void rgb_init();
void rgb_clear();
void rgb_color_red();
void rgb_color_green();
void rgb_color_blue();
void rgb_color_white();
void rgb_connecting();
void rgb_connected();
void rgb_provisioning();
void rgb_provisioned();
void rgb_error();

#endif