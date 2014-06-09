#include "TurboMicro.h"

/*////
// TurboSphere Icon
//
// It is 32x32, RGBA.
/*/

const char * turbo_icon =
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0KKK\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\11\11\11\12MMM\17\212\212\212\37\252\252\252I\321\321"
    "\321\364\307\307\307\315\0\0\0\3\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\14\14\14"
    "\36\246\246\246\177\311\311\311\303\340\340\340\334\334\334\334\340\261\261"
    "\261\347\315\315\315\365\352\352\352\375\340\340\340\325\0\0\0\3\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\33\33\33\0\0\0\0"
    "\0\0\0\0\1\253\253\2535\272\272\272\267\351\351\351\377\360\360\360\377\347"
    "\347\347\377\337\337\337\377\331\331\331\377\254\254\254\365\306\306\306"
    "\375\336\336\336\374\325\325\325\324\0\0\0\3\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0NNN\0\0\0\0\0\0\0\0\10\313\313\313\267\350\350"
    "\350\375\353\353\353\377\344\344\344\377\335\335\335\377\327\327\327\377"
    "\320\320\320\377\313\313\313\377\240\240\240\365\271\271\271\375\322\322"
    "\322\374\312\312\312\324\0\0\0\3\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0KKK\0\0\0\0\0\202\202\2021\314\314\314\365\354\354\354\377\342\342"
    "\342\377\334\334\334\377\326\326\326\377\317\317\317\377\311\311\311\377"
    "\303\303\303\377\274\274\274\377\225\225\225\365\254\254\254\375\310\310"
    "\310\374\300\300\300\324\0\0\0\3\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "EEE\0\0\0\0\0{{{9\331\331\331\366\343\343\343\377\333\333\333\377\324\324"
    "\324\377\316\316\316\377\307\307\307\377\301\301\301\377\273\273\273\377"
    "\264\264\264\377\257\257\257\377\210\210\210\365\237\237\237\375\275\275"
    "\275\374\266\266\266\324\0\0\0\3\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0MMM$\312\312\312\377\333\333\333\377\323\323\323\377\315\315\315"
    "\377\307\307\307\377\300\300\300\377\272\272\272\377\263\263\263\377\255"
    "\255\255\377\247\247\247\377\240\240\240\377|||\365\221\221\221\375\262\262"
    "\262\374\252\252\252\324\0\0\0\3\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0BBB\0GGG"
    "\15\300\300\300\330\324\324\324\377\317\317\317\377\314\314\314\377\306\306"
    "\306\377\277\277\277\377\267\267\267\377\256\256\256\377\246\246\246\377"
    "\237\237\237\377\230\230\230\377\223\223\223\377qqq\365\204\204\204\375\250"
    "\250\250\374\240\240\240\324\0\0\0\3\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0===\0\0\0\0\0"
    "\220\220\220i\310\310\310\377\256\256\256\377\202\202\202\377PPP\377EEE\377"
    "DDD\377NNN\377rrr\377\210\210\210\377\220\220\220\377\213\213\213\377\204"
    "\204\204\377eee\365www\375\234\234\234\374\226\226\226\324\0\0\0\3\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\11\261\261\261\376qqq\377YXX\377!\"\"\377"
    "\31\32\32\377\34\34\34\377\34\33\34\377\32\32\32\377\"\"\"\377WVV\377\\\\"
    "\\\377zzz\377www\377YYY\365iii\375\222\222\222\374\213\213\213\324\0\0\0"
    "\3\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0yyy\203iii\377AAA\377\13\13\13\377\"!\""
    "\377/-.\377323\377534\377.-.\377\"\"\"\377\13\13\13\377BBB\377PPP\377ggg"
    "\377MMM\365\\\\\\\375\207\207\207\374\201\201\201\324\0\0\0\3\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\30\30\30\12SSS\331AAA\377\27\27\27\377000\377<<<\377HFF\377"
    "TPP\377TPP\377HFF\377<<<\377000\377\27\26\27\377BAB\377MMM\377DDD\365TTT"
    "\375}}}\374www\324\0\0\0\3\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0""7772TTS\372\13\12\13"
    "\377000\377BBB\377TPP\377ZZZ\377ZZZ\377ZZZ\377ZZZ\377TPP\377BBB\377000\377"
    "\13\13\13\377UTU\377@@@\367RRR\375qqq\374mmm\324\0\0\0\3\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0FFF\217\"\"\"\377\"\"\"\377<<<\377TPP\377```\377```\377XXX\377XX"
    "X\377```\377```\377TPP\377<<<\377\"\"\"\377\"\"\"\377EEE\376QQQ\374fff\374"
    "bbb\324\0\0\0\3\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0EEE\374\32\32\32\377/-.\377HFF\377"
    "ZZZ\377```\377```\377\220\220\220\377\220\220\220\377```\377```\377ZZZ\377"
    "HFF\377/..\377\32\32\32\377DDD\377MMM\364\\\\\\\375XXX\325\0\0\0\3\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0HHH\377\34\34\34\377434\377TPP\377ZZZ\377XXX\377\220\220"
    "\220\377\330\330\330\377\330\330\330\377\220\220\220\377XXX\377ZZZ\377TP"
    "P\377323\377\35\34\34\377GGG\377444>EEE\377CCC\333\0\0\0\3\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\3\306\306\306\333"
    "\320\320\320\377\302\302\302>HHH\377\35\34\34\377323\377TPP\377ZZZ\377XX"
    "X\377\220\220\220\377\330\330\330\377\330\330\330\377\220\220\220\377XXX"
    "\377ZZZ\377TPP\377434\377\34\34\34\377HHH\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\3\340\340\340\325\352\352\352\375\323\323\323\364SSS\377\31\31\31\377"
    "/..\377HFF\377ZZZ\377```\377```\377\220\220\220\377\220\220\220\377```\377"
    "```\377ZZZ\377HFF\377/-.\377\32\32\32\377EEE\374\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\3\325\325\325\324\336\336\336\374\311\311\311\374|||\376\"\"\"\377"
    "\"\"\"\377<<<\377TPP\377```\377```\377XXX\377XXX\377```\377```\377TPP\377"
    "<<<\377\"\"\"\377\"\"\"\377FFF\217\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\3\312\312"
    "\312\324\322\322\322\374\271\271\271\375\216\216\216\367^]]\377\13\13\13"
    "\377000\377BBB\377TPP\377ZZZ\377ZZZ\377ZZZ\377ZZZ\377TPP\377BBB\377000\377"
    "\13\12\13\377TTS\3726662\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\3\300\300\300\324"
    "\310\310\310\374\254\254\254\375\237\237\237\365}}}\377@?@\377\27\26\27\377"
    "000\377<<<\377HFF\377TPP\377TPP\377HFF\377<<<\377000\377\27\26\27\377CBC"
    "\377FFF\331\0\0\0\12\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\3\266\266\266\324\275"
    "\275\275\374\237\237\237\375\223\223\223\365\322\322\322\377iii\377A@@\377"
    "\13\13\13\377\"\"\"\377/-.\377434\377323\377/..\377\"\"\"\377\13\13\13\377"
    "CCD\377LLL\377666\203\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\3\252\252\252"
    "\324\262\262\262\374\221\221\221\375\206\206\206\365\305\305\305\377\271"
    "\271\271\377jjj\377XWW\377\"\"\"\377\32\32\32\377\34\34\34\377\35\34\34\377"
    "\32\31\32\377\"\"\"\377UUU\377KKK\377JJJ\376\0\0\0\11\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\3\240\240\240\324\250\250\250\374\204\204\204\375{{{\365"
    "\270\270\270\377\254\254\254\377\242\242\242\377\211\211\211\377hhh\377J"
    "JJ\377EEE\377EEE\377HHH\377NNN\377NNN\377OOO\377888i\0\0\0\0\31\31\31\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\3\226\226\226\324\234\234\234\374www\375ooo"
    "\365\251\251\251\377\236\236\236\377\224\224\224\377\212\212\212\377\204"
    "\204\204\377{{{\377qqq\377fff\377\\\\\\\377SSS\377QQQ\377GGG\330\32\32\32"
    "\15\32\32\32\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\3\213\213\213\324\222"
    "\222\222\374iii\375bbb\365\234\234\234\377\220\220\220\377\206\206\206\377"
    "}}}\377sss\377iii\377___\377VVV\377QQQ\377PPP\377HHH\377\34\34\34$\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\3\201\201\201\324\207\207"
    "\207\374\\\\\\\375WWW\365\215\215\215\377\202\202\202\377xxx\377nnn\377d"
    "dd\377[[[\377SSS\377PPP\377PPP\377KKK\366***9\0\0\0\0\31\31\31\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\3www\324}}}\374TTT\375OOO\365\177"
    "\177\177\377ttt\377jjj\377aaa\377VVV\377QQQ\377PPP\377QQQ\377EEE\365,,,1"
    "\0\0\0\0\32\32\32\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\3mmm\324qqq\374RRR\375LLL\365qqq\377fff\377\\\\\\\377SSS\377PPP\377PPP\377"
    "MMM\375BBB\267\0\0\0\10\0\0\0\0\32\32\32\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\3bbb\324fff\374QQQ\375III\365ccc\377WWW"
    "\377RRR\377QQQ\377MMM\377===\2677775\0\0\0\1\0\0\0\0\15\15\15\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\3XXX\325\\\\"
    "\\\375MMM\365CCC\347QQQ\340MMM\334BBB\303666\177\5\5\5\36\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\3CCC\315FFF\364:::I111\37\30\30\30\17\1\1\1\12\0\0"
    "\0\0\0\0\0\0\0\0\0\0\32\32\32\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0";


const char *GetTurboSphereIcon(void) {
    return turbo_icon;
}
