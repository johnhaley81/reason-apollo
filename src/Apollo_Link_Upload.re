open Apollo_Types;

type uploadLinkOptions = {
  .
  "uri": Js.Nullable.t(string),
  "fetch": Js.Nullable.t(fetch),
  "fetchOptions": Js.Nullable.t(Js.t({.})),
  "credentials": Js.Nullable.t(string),
  "headers": Js.Nullable.t(Js.Json.t),
  "includeExtensions": Js.Nullable.t(bool),
};

/* Bind createUploadLink function from apollo upload link */
[@bs.module "apollo-upload-client"]
external make_: uploadLinkOptions => apolloLink = "createUploadLink";

/**
 * CreateUploadLink
 * https://github.com/jaydenseric/apollo-upload-client#function-createuploadlink
 */
let make =
    (
      ~uri=?,
      ~fetch=?,
      ~fetchOptions=?,
      ~credentials=?,
      ~headers=?,
      ~includeExtensions=?,
      (),
    ) =>
  make_(
    Js.Nullable.{
      "uri": fromOption(uri),
      "fetch": fromOption(fetch),
      "fetchOptions": fromOption(fetchOptions),
      "credentials": fromOption(credentials),
      "headers": fromOption(headers),
      "includeExtensions": fromOption(includeExtensions),
    },
  );
