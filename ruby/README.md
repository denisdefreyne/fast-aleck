# Fast Aleck Ruby bindings

This library contains Ruby bindings (using FFI) for Fast Aleck. Fast Aleck is a tool for making text look smarter by using proper Unicode characters. For example, ... becomes … (`&hellip;`) and -- becomes — (`&mdash;`). Its goal is to be the fastest text cleaning tool out there.

The main module is {FastAleck}. The only public function is {FastAleck.process}. The FFI bindings are located in {FastAleck::C}.

Note: This documentation looks best with Yardoc, not RDoc.

## Dependencies

Only FFI. You will obviously also need to have Fast Aleck installed.

## Contact

You can reach me at <denis.defreyne@stoneship.org>.
