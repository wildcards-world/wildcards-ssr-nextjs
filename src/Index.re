[%bs.raw {|require("react-tabs/style/react-tabs.css")|}];

open Globals;

module Router = {
  [@react.component]
  let make = () => {
    let url = ReasonReactRouter.useUrl();
    switch (url.path) {
    | [_] => <p> {React.string("Unknown page")} </p>
    | _ => <ReactTranslate> <Layout /> </ReactTranslate>
    };
  };
};

[@bs.val]
external mainnetApi: option(string) = "process.env.REACT_APP_MAINNET_BE";
[@bs.val]
external goerliApi: option(string) = "process.env.REACT_APP_GOERLI_BE";

ReactDOMRe.renderToElementWithId(
  <WildcardsProvider
    getGraphEndpoints={(networkId, ()) => {
      Js.log2("YOU ARE USING net ID:", networkId);
      Js.log2("mainnetApi", mainnetApi);
      Js.log2("goerliApi", goerliApi);
      let endpoints =
        switch (networkId) {
        | 5 => (
            goerliApi |||| "https://goerli.api.wildcards.world/v1/graphq",
            "wss://api.thegraph.com/subgraphs/name/wildcards-world/wildcards-goerli",
          )
        | _ => (
            mainnetApi |||| "https://api.wildcards.world/v1/graphql",
            "wss://api.thegraph.com/subgraphs/name/wildcards-world/wildcards",
          )
        };
      Js.log(endpoints);
      endpoints;
    }}>
    <UsdPriceProvider> <Router /> </UsdPriceProvider>
  </WildcardsProvider>,
  "root",
);
