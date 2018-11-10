open Apollo_Types;

[@bs.deriving abstract]
type persistCacheOptions = {
  cache: apolloCache,
  storage,
  [@bs.optional]
  /* TODO: handle all option types with a variant */
  trigger: string,
  [@bs.optional]
  debounce: int,
  [@bs.optional]
  key: string,
  [@bs.optional]
  serialize: bool,
  [@bs.optional]
  /* TODO: handle numbers */
  maxSize: bool,
  [@bs.optional]
  debug: bool,
};

[@bs.module "apollo-cache-persist"]
external persistCache: persistCacheOptions => Js.Promise.t(unit) = "";

let make =
    (
      ~cache,
      ~storage,
      ~trigger=?,
      ~debounce=?,
      ~key=?,
      ~serialize=?,
      ~maxSize=?,
      ~debug=?,
      (),
    ) =>
  persistCache(
    persistCacheOptions(
      ~cache,
      ~storage,
      ~trigger?,
      ~debounce?,
      ~key?,
      ~serialize?,
      ~maxSize?,
      ~debug?,
      (),
    ),
  );
