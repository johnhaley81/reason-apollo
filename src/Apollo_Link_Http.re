open Apollo_Types;

type linkOptions = {
  .
  "uri": string,
  "includeExtensions": Js.Nullable.t(bool),
  "fetch": Js.Nullable.t(fetch),
  "headers": Js.Nullable.t(Js.Json.t),
  "credentials": Js.Nullable.t(string),
  "fetchOptions": Js.Nullable.t(Js.Json.t),
};

/* Bind the HttpLink class */
[@bs.module "apollo-link-http"] [@bs.new]
external make_: linkOptions => apolloLink = "HttpLink";

/**
 * CreateHttpLink
 * https://github.com/apollographql/apollo-link/tree/master/packages/apollo-link-http
 */
let make =
    (
      ~uri,
      ~includeExtensions=?,
      ~fetch=?,
      ~headers=?,
      ~credentials=?,
      ~fetchOptions=?,
      (),
    ) =>
  make_({
    "uri": uri,
    "includeExtensions": Js.Nullable.fromOption(includeExtensions),
    "fetch": Js.Nullable.fromOption(fetch),
    "headers": Js.Nullable.fromOption(headers),
    "credentials": Js.Nullable.fromOption(credentials),
    "fetchOptions": Js.Nullable.fromOption(fetchOptions),
  });
